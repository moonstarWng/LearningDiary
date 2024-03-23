#include "../include/main.h"
void sha256_file(const char *path, char *hash_string) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    SHA256_CTX context;
    SHA256_Init(&context);

    unsigned char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, 4096, file)) > 0) {
        SHA256_Update(&context, buffer, bytes_read);
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &context);

    fclose(file);

    // 将哈希值转换为字符串
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash_string + 2 * i, "%02x", hash[i]);
    }
    hash_string[2 * SHA256_DIGEST_LENGTH] = '\0';  // 添加字符串结尾的空字符 '\0'
}
