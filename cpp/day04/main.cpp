#include <openssl/md5.h>

#include <advent.hpp>
#include <cstdio>
#include <cstring>

int main() {
  std::string input = advent::readEntireFile("day04/input.txt");
  bool part1 = false;
  for (int i = 1; i < 10'000'000; i++) {
    char str[100];
    int len = snprintf(str, 100, "%s%d", input.c_str(), i);
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)str, len, md5);
    char strmd5[2 * MD5_DIGEST_LENGTH + 1];
    int strmd5len = 0;
    for (unsigned char c : md5) {
      strmd5len += snprintf(strmd5 + strmd5len,
                            2 * MD5_DIGEST_LENGTH - strmd5len, "%02x", c);
    }
    if (!part1 && strncmp(strmd5, "00000", 5) == 0) {
      printf("Part 1: %d\n", i);
      part1 = true;
    }
    if (strncmp(strmd5, "000000", 6) == 0) {
      printf("Part 2: %d\n", i);
      break;
    }
  }
}
