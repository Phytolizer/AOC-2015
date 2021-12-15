#include <advent.hpp>
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

constexpr bool DEBUG_OUTPUT = false;

class Gate
{
  public:
    using SignalStrength = std::uint16_t;

#define GATE_OPS_                                                                                                      \
    X(SET)                                                                                                             \
    X(NOT)                                                                                                             \
    X(RSHIFT)                                                                                                          \
    X(LSHIFT)                                                                                                          \
    X(AND)                                                                                                             \
    X(OR)

    enum class Op
    {
#define X(x) x,
        GATE_OPS_
#undef X
    };

  private:
    static std::string_view opStr(Op op)
    {
        switch (op)
        {
#define X(x)                                                                                                           \
    case Op::x:                                                                                                        \
        return #x;
            GATE_OPS_
#undef X
        }

        throw std::runtime_error{"corrupt op"};
    }

    std::string m_name;
    Op m_op;
    std::array<std::string, 2> m_input;
    std::string m_description;
    std::optional<SignalStrength> m_value1;
    std::optional<SignalStrength> m_bits;

    static constexpr ctll::fixed_string WIRE_PATTERN = R"(^[a-z]+$)";

    template <typename T> T parse(std::string_view str)
    {
        std::istringstream ss{std::string{str}};
        T value;
        ss >> value;
        return value;
    }

  public:
    Gate() : m_op{Op::SET}
    {
    }

    Gate(std::string name, Op op, std::array<std::string, 2> input)
        : m_name{std::move(name)}, m_op{op}, m_input{std::move(input)}
    {
        if (m_input[0].empty())
        {
            m_description = fmt::format("{} {}", opStr(op), m_input[1]);
        }
        else
        {
            if (m_input[1].empty())
            {
                m_description = m_input[0];
            }
            else
            {
                m_description = fmt::format("{} {} {}", m_input[0], opStr(op), m_input[1]);
            }
        }

        switch (op)
        {
        case Op::SET:
            if (!ctre::match<WIRE_PATTERN>(m_input[0]))
            {
                m_value1 = {parse<SignalStrength>(m_input[0])};
            }
            break;
        case Op::LSHIFT:
        case Op::RSHIFT:
            if (!ctre::match<WIRE_PATTERN>(m_input[0]))
            {
                throw std::runtime_error{"invalid shift"};
            }
            m_bits = {parse<SignalStrength>(m_input[1])};
            break;
        case Op::NOT:
            if (!m_input[0].empty() || !ctre::match<WIRE_PATTERN>(m_input[1]))
            {
                throw std::runtime_error{"invalid not"};
            }
            break;
        case Op::AND:
        case Op::OR:
            if (!ctre::match<WIRE_PATTERN>(m_input[1]))
            {
                throw std::runtime_error{"invalid and/or"};
            }
            if (!ctre::match<WIRE_PATTERN>(m_input[0]))
            {
                m_value1 = {parse<SignalStrength>(m_input[0])};
            }
            break;
        }
    }

    std::string name() const
    {
        return m_name;
    }

    SignalStrength eval(std::unordered_map<std::string, Gate>* gates,
                        std::unordered_map<std::string, SignalStrength>* cache, size_t depth)
    {
        if (DEBUG_OUTPUT)
        {
            fmt::print("{:{}}eval {} => {}\n", "", depth * 2, m_name, m_description);
        }
        if (cache->contains(m_name))
        {
            if (DEBUG_OUTPUT)
            {
                fmt::print("{:{}}=> {} (cached)\n", "", depth * 2, cache->at(m_name));
            }
            return cache->at(m_name);
        }
        switch (m_op)
        {
        case Op::SET:
            if (m_value1)
            {
                return *m_value1;
            }
            else
            {
                SignalStrength result = gates->at(m_input[0]).eval(gates, cache, depth + 1);
                cache->emplace(m_name, result);
                if (DEBUG_OUTPUT)
                {
                    fmt::print("{:{}}=> {}\n", "", depth * 2, result);
                }
                return result;
            }
        case Op::NOT: {
            SignalStrength result = ~gates->at(m_input[1]).eval(gates, cache, depth + 1);
            cache->emplace(m_name, result);
            if (DEBUG_OUTPUT)
            {
                fmt::print("{:{}}=> {}\n", "", depth * 2, result);
            }
            return result;
        }
        case Op::RSHIFT: {
            SignalStrength result = gates->at(m_input[0]).eval(gates, cache, depth + 1) >> *m_bits;
            cache->emplace(m_name, result);
            if (DEBUG_OUTPUT)
            {
                fmt::print("{:{}}=> {}\n", "", depth * 2, result);
            }
            return result;
        }
        case Op::LSHIFT: {
            SignalStrength result = gates->at(m_input[0]).eval(gates, cache, depth + 1) << *m_bits;
            cache->emplace(m_name, result);
            if (DEBUG_OUTPUT)
            {
                fmt::print("{:{}}=> {}\n", "", depth * 2, result);
            }
            return result;
        }
        case Op::AND:
            if (m_value1)
            {
                SignalStrength result = *m_value1 & gates->at(m_input[1]).eval(gates, cache, depth + 1);
                cache->emplace(m_name, result);
                if (DEBUG_OUTPUT)
                {
                    fmt::print("{:{}}=> {}\n", "", depth * 2, result);
                }
                return result;
            }
            else
            {
                SignalStrength result = gates->at(m_input[0]).eval(gates, cache, depth + 1) &
                                        gates->at(m_input[1]).eval(gates, cache, depth + 1);
                cache->emplace(m_name, result);
                if (DEBUG_OUTPUT)
                {
                    fmt::print("{:{}}=> {}\n", "", depth * 2, result);
                }
                return result;
            }
        case Op::OR:
            if (m_value1)
            {
                SignalStrength result = *m_value1 | gates->at(m_input[1]).eval(gates, cache, depth + 1);
                cache->emplace(m_name, result);
                if (DEBUG_OUTPUT)
                {
                    fmt::print("{:{}}=> {}\n", "", depth * 2, result);
                }
                return result;
            }
            else
            {
                SignalStrength result = gates->at(m_input[0]).eval(gates, cache, depth + 1) |
                                        gates->at(m_input[1]).eval(gates, cache, depth + 1);
                cache->emplace(m_name, result);
                if (DEBUG_OUTPUT)
                {
                    fmt::print("{:{}}=> {}\n", "", depth * 2, result);
                }
                return result;
            }
        }
    }
};

std::optional<Gate> parseLine(std::string line)
{
    auto [matched, inputs, rawName] = ctre::match<R"((.*) -> (.*))">(line);
    if (!matched)
    {
        return std::nullopt;
    }
    std::istringstream iss(std::string{std::string_view{inputs}});
    std::vector<std::string> words{std::istream_iterator<std::string>{iss}, {}};
    auto name = std::string{std::string_view{rawName}};
    switch (words.size())
    {
    case 1:
        return Gate{name, Gate::Op::SET, {words[0], ""}};
    case 2:
        if (words[0] == "NOT")
        {
            return Gate{name, Gate::Op::NOT, {"", words[1]}};
        }
        else if (words[0] == "RSHIFT")
        {
            return Gate{name, Gate::Op::RSHIFT, {"", words[1]}};
        }
        else if (words[0] == "LSHIFT")
        {
            return Gate{name, Gate::Op::LSHIFT, {"", words[1]}};
        }
        else if (words[0] == "AND")
        {
            return Gate{name, Gate::Op::AND, {"", words[1]}};
        }
        else if (words[0] == "OR")
        {
            return Gate{name, Gate::Op::OR, {"", words[1]}};
        }
        else
        {
            return std::nullopt;
        }
    case 3:
        if (words[1] == "NOT")
        {
            return Gate{name, Gate::Op::NOT, {words[0], words[2]}};
        }
        else if (words[1] == "RSHIFT")
        {
            return Gate{name, Gate::Op::RSHIFT, {words[0], words[2]}};
        }
        else if (words[1] == "LSHIFT")
        {
            return Gate{name, Gate::Op::LSHIFT, {words[0], words[2]}};
        }
        else if (words[1] == "AND")
        {
            return Gate{name, Gate::Op::AND, {words[0], words[2]}};
        }
        else if (words[1] == "OR")
        {
            return Gate{name, Gate::Op::OR, {words[0], words[2]}};
        }
        else
        {
            return std::nullopt;
        }
    default:
        return std::nullopt;
    }
}

int main()
{
    std::unordered_map<std::string, Gate> gates;
    std::ifstream input{"day07/input.txt"};
    std::string line;
    ADVENT_FOREACH_LINE(input, line)
    {
        if (auto gate = parseLine(line); gate)
        {
            gates.emplace(gate->name(), *gate);
        }
    }

    std::unordered_map<std::string, Gate::SignalStrength> cache;
    Gate::SignalStrength a = gates.at("a").eval(&gates, &cache, 0);
    fmt::print("a: {}\n", a);
    gates["b"] = Gate{"b", Gate::Op::SET, {std::to_string(a), ""}};
    cache.clear();
    fmt::print("part2: {}\n", gates.at("a").eval(&gates, &cache, 0));
}
