#include <vector>
#include <string>
#include <iostream>
#include <limits>


using namespace std;

vector<byte> beaufortCipher(const vector<byte>& data, const vector<byte>& key) {
    vector<byte> result;
    result.reserve(data.size());
    int alphabetSize = 256;
    for (size_t i = 0; i < data.size(); ++i) {
        int encrypted = (static_cast<int>(key[i % key.size()])
                       - static_cast<int>(data[i]) + alphabetSize) % alphabetSize;
        result.push_back(static_cast<byte>(encrypted));
    }
    return result;
}

string beaufortCipher(const string& text, const vector<byte>& key) {
    string result;
    result.reserve(text.size());
    const int alphabetSize = 256;

    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char text_char = static_cast<int>(text[i]) + alphabetSize;
        unsigned char key_char = static_cast<int>(key[i % key.size()]);

        // Применяем формулу Бофора
        int encrypted = (key_char - text_char + alphabetSize) % alphabetSize;
        result.push_back(static_cast<char>(encrypted));
    }

    return result;
}
extern "C" {
	void beaufortEncryptFile(const vector<byte>& text, const vector<byte>& key, vector<byte>& result) {
		result = beaufortCipher(text, key);
	}
	void beaufortDecryptFile(const vector<byte>& text, const vector<byte>& key, vector<byte>& result) {
		result = beaufortCipher(text, key);
	}
	void beaufortEncryptString(const string& text, const vector<byte>& key, string& result) {
		result = beaufortCipher(text, key);
	}
	void beaufortDecryptString(const string& text, const vector<byte>& key, string& result) {
		result = beaufortCipher(text, key);
	}
}

