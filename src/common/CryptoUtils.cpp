#include "CryptoUtils.h"

CTR<AES128> AES128CTRCipher;

void generateIV(uint8_t* iv) {
  for(uint8_t i = 0; i < 4; i++) {
    uint32_t randomNum = RANDOM_REG32;
    uint8_t offset = i * 4;
    iv[offset] = randomNum & 0xFF;
    randomNum = (randomNum >> 8);
    iv[offset + 1] = randomNum & 0xFF;
    randomNum = (randomNum >> 8);
    iv[offset + 2] = randomNum & 0xFF;
    randomNum = (randomNum >> 8);
    iv[offset + 3] = randomNum & 0xFF;
  }
}

uint8_t chr2Nibble(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'a' && ch <= 'f')
    return (ch - 'a') + 10;
  if (ch >= 'A' && ch <= 'F')
    return (ch - 'A') + 10;
  return 0;
}

char nibble2Chr(uint8_t nibble) {
  nibble = nibble & 0x0f;
  if(nibble >= 0 && nibble <= 9) {
    return '0' + nibble;
  }
  if(nibble > 9 && nibble < 16) {
    return (nibble - 10) + 'A';
  }
  return '0';
}

size_t hexStrToByteArr(const char* hexStr, uint8_t* bytes) {
  size_t len = strlen(hexStr);
  size_t byteLen = len / 2;

  for(size_t i = 0; i < byteLen; i++) {
    size_t hexIdx = i * 2;
    bytes[i] = (chr2Nibble(hexStr[hexIdx]) << 4) + chr2Nibble(hexStr[hexIdx + 1]);
  }

  return byteLen;
}

String byteArrToHexStr(uint8_t* bytes, size_t len) {
  String str = "";
  for(size_t i = 0; i < len; i++) {
    str = str + nibble2Chr(bytes[i] >> 4);
    str = str + nibble2Chr(bytes[i]);
  }
  return str;
}

String encrypt(const char* plainText, const char* key) {
  uint8_t keyBytes[16];
  hexStrToByteArr(key, keyBytes);
  return encrypt(plainText, keyBytes);
}

String encrypt(const char* plainText, uint8_t* key) {
  uint8_t iv[16];
  generateIV(iv);
  AES128CTRCipher.clear();

  AES128CTRCipher.setIV(iv, 16);
  AES128CTRCipher.setKey(key, 16);

  size_t len = strlen(plainText);
  if(len % 16 != 0) {
    len = len + (16 - (len % 16));
  }
  uint8_t* input = new uint8_t[len];
  size_t i = 0;
  for(;plainText[i] != '\0'; i++) {
    input[i] = plainText[i];
  }
  for(;i < len; i++) {
    input[i] = ' ';
  }

  uint8_t* output = new uint8_t[len];
  AES128CTRCipher.encrypt(output, input, len);
  String cipherText = byteArrToHexStr(iv, 16) + '-' + byteArrToHexStr(output, len);
  delete input;
  delete output;
  return cipherText;
}

String decrypt(const char* cipherText, const char* key) {
  uint8_t keyBytes[16];
  hexStrToByteArr(key, keyBytes);
  return decrypt(cipherText, keyBytes);
}

String decrypt(const char* cipherText, uint8_t* key) {
  char ivStr[33];
  for(uint8_t i = 0; i < 32; i++) {
    ivStr[i] = cipherText[i];
  }
  ivStr[32] = '\0';

  uint8_t iv[16];
  hexStrToByteArr(ivStr, iv);

  size_t totalLen = strlen(cipherText);
  size_t cipherLen = totalLen - 33; // total has: iv(32) + hyphen(1) + cipher(rest)
  uint8_t* input = new uint8_t[cipherLen + 1];
  for(size_t i = 0; i < cipherLen; i++) {
    input[i] = cipherText[i + 33];
  }
  input[cipherLen] = '\0';
  cipherLen = hexStrToByteArr((char*)input, input);

  uint8_t* output = new uint8_t[cipherLen];

  CTR<AESSmall128> AES128CTRCipher;
  AES128CTRCipher.clear();
  AES128CTRCipher.setIV(iv, 16);
  AES128CTRCipher.setKey(key, 16);
  AES128CTRCipher.decrypt(output, input, cipherLen);
  String result = "";
  for(size_t i = 0; i < cipherLen; i++) {
    result += (char)(output[i]);
  }
  delete input;
  delete output;
  return result;
}
