#include <fstream>
#include <iostream>
#include <string>
#include <vector>


using namespace std;

// Алгоритм XOR для строки
string xorCipher(const string& text, const vector<byte>& key) {
    string result;
    for (size_t i = 0; i < text.size(); ++i) {
        auto textСhar = static_cast<unsigned char>(text[i]);
        auto keyСhar = static_cast<unsigned char>(key[i % key.size()]);
        result.push_back(static_cast<char>(textСhar ^ keyСhar));
    }

    return result;
}
	// Алгоритм XOR для файла
vector<byte> xorCipher(const vector<byte>& data, const vector<byte>& key) {
    vector<byte> result;
    result.reserve(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        byte encrypted_byte = data[i] ^ key[i % key.size()];
        result.push_back(encrypted_byte);
    }
    return result;
}

extern "C" {
	void xorEncryptFile(const vector<byte>& text, const vector<byte>& key, vector<byte>& result) {
		result = xorCipher(text, key);
	}
	void xorDecryptFile(const vector<byte>& text, const vector<byte>& key, vector<byte>& result) {
		result = xorCipher(text, key);
	}
	void xorEncryptString(const string& text, const vector<byte>& key, string& result) {
		result = xorCipher(text, key);
	}
	void xorDecryptString(const string& text, const vector<byte>& key, string& result) {
		result = xorCipher(text, key);
	}
}
