#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

static const int N = 6;

int main(int argc, char* argv[]) {
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, argv[1], strlen(argv[1]));

  int M = 1;
  for (int i = 0; i < N; i++)
    M *= 26;

  char buf[99] = {};
  unsigned char md[21];
  for (int i = 0; i < M; i++) {
    int v = i;
    for (int j = 0; j < N; j++) {
      buf[j] = 'a' + v % 26;
      v /= 26;
    }

    SHA_CTX c = ctx;
    SHA1_Update(&c, buf, N);
    SHA1_Final(md, &c);
    if (!memcmp(md, "\x12\x34\x56", 3)) {
      puts(buf);
      break;
    }
  }
}
