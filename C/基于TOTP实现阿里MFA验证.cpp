// ����TOTPʵ�ְ���MFA��֤.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <openssl/hmac.h> // OpenSSL HMAC����

#pragma comment(lib, "libcrypto.lib")

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Crypt32")

// Base32�����RFC4648��׼��
static const char base32_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '2', '3', '4', '5', '6', '7'
};

/**
 * Base32���뺯��
 * @param encoded ����Base32�ַ���
 * @param result ����ֽ�����
 * @return �������ֽ�����-1��ʾ����
 */
int base32_decode(const char *encoded, uint8_t *result) {
    int buffer = 0, bits = 0, count = 0;
    for (const char *p = encoded; *p; ++p) {
        if (*p == '=') break; // ��������
        const char *c = strchr(base32_table, *p);
        if (!c) return -1;    // �Ƿ��ַ�
        
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
 * ����TOTP�루6λ��
 * @param secret Base32�������Կ
 * @return 6λ��̬�����ַ���
 */
char* generate_totp(const char *secret) {
    uint8_t key[20]; // ��������Կ
    int key_len = base32_decode(secret, key);
    if (key_len < 0) return NULL;

    // ��ȡ��ǰʱ�����30��Ϊһ��ʱ�䴰�ڣ�
    time_t timestamp = time(NULL);
    uint64_t time_step = (uint64_t)timestamp / 30;

    // ��ʱ�䲽��ת��Ϊ����ֽ���
    uint8_t msg[8];
    for (int i = 7; i >= 0; --i) {
        msg[i] = time_step & 0xFF;
        time_step >>= 8;
    }

    // ʹ��OpenSSL����HMAC-SHA1
    uint8_t hash[20];
    unsigned int hash_len;
    HMAC(EVP_sha1(), key, key_len, msg, 8, hash, &hash_len);

    // ��̬�ضϣ�DT�㷨��
    int offset = hash[19] & 0x0F;
    uint32_t code = (hash[offset] & 0x7F) << 24 |
                    (hash[offset+1] & 0xFF) << 16 |
                    (hash[offset+2] & 0xFF) << 8 |
                    (hash[offset+3] & 0xFF);

    // ����6λ����
    static char otp[7];
    _snprintf_s(otp, sizeof(otp), "%06d", code % 1000000);
    return otp;
}

// ʾ���÷�
int main()
{
	while(true)
	{
		const char *secret = "JBSWY3DPEHPK3PXP"; // ʾ����Կ��Base32���룩
		char *code = generate_totp(secret);
		printf("Current TOTP: %s\n", code);
		system("pause");
	}

    return 0;
}