#include <vector>
#include <random>
#include <stdexcept>

#include "support.h"

using namespace std;

vector<byte> generateKey() {
    vector<byte> key(16);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 255);

    for (auto& b : key) {
        b = static_cast<byte>(distrib(gen));
    }
    return key;
}

string bytesToHex(const vector<byte>& data) {
    string hex;
    for (byte b : data) {
        hex += "0123456789ABCDEF"[static_cast<unsigned char>(b) >> 4];
        hex += "0123456789ABCDEF"[static_cast<unsigned char>(b) & 0x0F];
    }
    return hex;
}

vector<byte> hexToVector(const string& hex) {
    vector<byte> result;

    if (hex.length() % 2 != 0) {
        throw invalid_argument("Hex-строка должна содержать четное количество символов");
    }

    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);

        if (!isxdigit(byteString[0]) || !isxdigit(byteString[1])) {
            throw invalid_argument("Недопустимый hex-символ: " + byteString);
        }

        result.push_back(static_cast<byte>(stoul(byteString, nullptr, 16)));
    }

    return result;
}

string toHex(const string &input) {
    static const char hex_digits[] = "0123456789ABCDEF";
    string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input) {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

string hexToString(const string& hex) {
    string result;

    if (hex.length() % 2 != 0) {
        throw invalid_argument("Hex-строка должна содержать четное количество символов");
    }

    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);

        if (!isxdigit(byteString[0]) || !isxdigit(byteString[1])) {
            throw invalid_argument("Недопустимый hex-символ: " + byteString);
        }

        result.push_back(static_cast<char>(stoul(byteString, nullptr, 16)));
    }

    return result;
}

