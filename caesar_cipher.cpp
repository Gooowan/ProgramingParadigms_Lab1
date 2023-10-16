// g++ -dynamiclib -o caesar_cipher.dylib caesar_cipher.cpp

#include <iostream>
#include <cstring>
using namespace std;

extern "C" char* encrypt(const char* rawText, int key) {
    size_t length = strlen(rawText);
    char* encryptedText = new char[length + 1];
    key = ((key % 26) + 26) % 26;
    cout << "key: " << key << endl;
    for (size_t i = 0; i < length; i++) {
        char originalChar = rawText[i];
        char encryptedChar = originalChar;


        if (originalChar >= 'a' && originalChar <= 'z') {
            encryptedChar = 'a' + (originalChar - 'a' + key) % 26;
        } else if (originalChar >= 'A' && originalChar <= 'Z') {
            encryptedChar = 'A' + (originalChar - 'A' + key) % 26;
        }

        encryptedText[i] = encryptedChar;
    }
    encryptedText[length] = '\0';

    return encryptedText;
}

extern "C" char* decrypt(const char* encryptedText, int key) {
    size_t length = strlen(encryptedText);
    char* decryptedText = new char[length + 1];
    key = ((key % 26) + 26) % 26;
    cout << "key: " << key << endl;

    for (size_t i = 0; i < length; i++) {
        char encryptedChar = encryptedText[i];
        char decryptedChar = encryptedChar;


        if (encryptedChar >= 'a' && encryptedChar <= 'z') {
            decryptedChar = 'a' + (encryptedChar - 'a' - key + 26) % 26;
        } else if (encryptedChar >= 'A' && encryptedChar <= 'Z') {
            decryptedChar = 'A' + (encryptedChar - 'A' - key + 26) % 26;
        }

        decryptedText[i] = decryptedChar;
    }
    decryptedText[length] = '\0';

    return decryptedText;
}
