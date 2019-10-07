#include "Arduino.h"
#include "Crypto.h"
#include "AES.h"
#include "CTR.h"

void generateIV(uint8_t*);
size_t hexStrToByteArr(const char* hexStr, uint8_t* bytes);
String byteArrToHexStr(uint8_t* bytes, size_t len);

String encrypt(const char* plainText, const char* key);
String encrypt(const char* plainText, uint8_t* key);
String decrypt(const char* cipherText, const char* key);
String decrypt(const char* cipherText, uint8_t* key);
