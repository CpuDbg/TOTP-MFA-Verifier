// 基于TOTP实现阿里MFA验证.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h> // OpenSSL HMAC函数

#pragma comment(lib, "libcrypto.lib")

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Crypt32")

// Base32解码表（RFC4648标准）
static const char base32_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '2', '3', '4', '5', '6', '7'
};

/**
 * Base32解码函数
 * @param encoded 输入Base32字符串
 * @param result 输出字节数组
 * @return 解码后的字节数，-1表示错误
 */
int base32_decode(const char *encoded, uint8_t *result) {
    int buffer = 0, bits = 0, count = 0;
    for (const char *p = encoded; *p; ++p) {
        if (*p == '=') break; // 忽略填充符
        const char *c = strchr(base32_table, *p);
        if (!c) return -1;    // 非法字符
        
        buffer = (buffer << 5) | (c - base32_table);
        bits += 5;
        if (bits >= 8) {
            bits -= 8;
            result[count++] = (buffer >> bits) & 0xFF;
        }
    }
    return count;
}

/**
 * 生成TOTP码（6位）
 * @param secret Base32编码的密钥
 * @return 6位动态密码字符串
 */
char* generate_totp(const char *secret) {
    uint8_t key[20]; // 解码后的密钥
    int key_len = base32_decode(secret, key);
    if (key_len < 0) return NULL;

    // 获取当前时间戳（30秒为一个时间窗口）
    time_t timestamp = time(NULL);
    uint64_t time_step = (uint64_t)timestamp / 30;

    // 将时间步长转换为大端字节序
    uint8_t msg[8];
    for (int i = 7; i >= 0; --i) {
        msg[i] = time_step & 0xFF;
        time_step >>= 8;
    }

    // 使用OpenSSL计算HMAC-SHA1
    uint8_t hash[20];
    unsigned int hash_len;
    HMAC(EVP_sha1(), key, key_len, msg, 8, hash, &hash_len);

    // 动态截断（DT算法）
    int offset = hash[19] & 0x0F;
    uint32_t code = (hash[offset] & 0x7F) << 24 |
                    (hash[offset+1] & 0xFF) << 16 |
                    (hash[offset+2] & 0xFF) << 8 |
                    (hash[offset+3] & 0xFF);

    // 生成6位数字
    static char otp[7];
    _snprintf_s(otp, sizeof(otp), "%06d", code % 1000000);
    return otp;
}

// 示例用法
int main()
{
	while(true)
	{
		const char *secret = "JBSWY3DPEHPK3PXP"; // 示例密钥（Base32编码）
		char *code = generate_totp(secret);
		printf("Current TOTP: %s\n", code);
		system("pause");
	}

    return 0;
}