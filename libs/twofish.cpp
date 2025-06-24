#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <string>
#include <cstddef>

using namespace std;


constexpr int BLOCK_SIZE = 16;
constexpr int MAX_ROUNDS = 16;
constexpr int RS_GF_FDBK = 0x14D;
constexpr int SK_STEP = 0x02020202; // 0x01010101 * 2
constexpr int SK_ROTL = 9;

// Q-box 
const vector<byte> Q0 = {
    byte{0xA9}, byte{0x67}, byte{0xB3}, byte{0xE8}, byte{0x04}, byte{0xFD}, byte{0xA3}, byte{0x76},
    byte{0x9A}, byte{0x92}, byte{0x80}, byte{0x78}, byte{0xE4}, byte{0xDD}, byte{0xD1}, byte{0x38},
    byte{0x0D}, byte{0xC6}, byte{0x35}, byte{0x98}, byte{0x18}, byte{0xF7}, byte{0xEC}, byte{0x6C},
    byte{0x43}, byte{0x75}, byte{0x37}, byte{0x26}, byte{0xFA}, byte{0x13}, byte{0x94}, byte{0x48},
    byte{0xF2}, byte{0xD0}, byte{0x8B}, byte{0x30}, byte{0x84}, byte{0x54}, byte{0xDF}, byte{0x23},
    byte{0x19}, byte{0x5B}, byte{0x3D}, byte{0x59}, byte{0xF3}, byte{0xAE}, byte{0xA2}, byte{0x82},
    byte{0x63}, byte{0x01}, byte{0x83}, byte{0x2E}, byte{0xD9}, byte{0x51}, byte{0x9B}, byte{0x7C},
    byte{0xA6}, byte{0xEB}, byte{0xA5}, byte{0xBE}, byte{0x16}, byte{0x0C}, byte{0xE3}, byte{0x61},
    byte{0xC0}, byte{0x8C}, byte{0x3A}, byte{0xF5}, byte{0x73}, byte{0x2C}, byte{0x25}, byte{0x0B},
    byte{0xBB}, byte{0x4E}, byte{0x89}, byte{0x6B}, byte{0x53}, byte{0x6A}, byte{0xB4}, byte{0xF1},
    byte{0xE1}, byte{0xE6}, byte{0xBD}, byte{0x45}, byte{0xE2}, byte{0xF4}, byte{0xB6}, byte{0x66},
    byte{0xCC}, byte{0x95}, byte{0x03}, byte{0x56}, byte{0xD4}, byte{0x1C}, byte{0x1E}, byte{0xD7},
    byte{0xFB}, byte{0xC3}, byte{0x8E}, byte{0xB5}, byte{0xE9}, byte{0xCF}, byte{0xBF}, byte{0xBA},
    byte{0xEA}, byte{0x77}, byte{0x39}, byte{0xAF}, byte{0x33}, byte{0xC9}, byte{0x62}, byte{0x71},
    byte{0x81}, byte{0x79}, byte{0x09}, byte{0xAD}, byte{0x24}, byte{0xCD}, byte{0xF9}, byte{0xD8},
    byte{0xE5}, byte{0xC5}, byte{0xB9}, byte{0x4D}, byte{0x44}, byte{0x08}, byte{0x86}, byte{0xE7},
    byte{0xA1}, byte{0x1D}, byte{0xAA}, byte{0xED}, byte{0x06}, byte{0x70}, byte{0xB2}, byte{0xD2},
    byte{0x41}, byte{0x7B}, byte{0xA0}, byte{0x11}, byte{0x31}, byte{0xC2}, byte{0x27}, byte{0x90},
    byte{0x20}, byte{0xF6}, byte{0x60}, byte{0xFF}, byte{0x96}, byte{0x5C}, byte{0xB1}, byte{0xAB},
    byte{0x9E}, byte{0x9C}, byte{0x52}, byte{0x1B}, byte{0x5F}, byte{0x93}, byte{0x0A}, byte{0xEF},
    byte{0x91}, byte{0x85}, byte{0x49}, byte{0xEE}, byte{0x2D}, byte{0x4F}, byte{0x8F}, byte{0x3B},
    byte{0x47}, byte{0x87}, byte{0x6D}, byte{0x46}, byte{0xD6}, byte{0x3E}, byte{0x69}, byte{0x64},
    byte{0x2A}, byte{0xCE}, byte{0xCB}, byte{0x2F}, byte{0xFC}, byte{0x97}, byte{0x05}, byte{0x7A},
    byte{0xAC}, byte{0x7F}, byte{0xD5}, byte{0x1A}, byte{0x4B}, byte{0x0E}, byte{0xA7}, byte{0x5A},
    byte{0x28}, byte{0x14}, byte{0x3F}, byte{0x29}, byte{0x88}, byte{0x3C}, byte{0x4C}, byte{0x02},
    byte{0xB8}, byte{0xDA}, byte{0xB0}, byte{0x17}, byte{0x55}, byte{0x1F}, byte{0x8A}, byte{0x7D},
    byte{0x57}, byte{0xC7}, byte{0x8D}, byte{0x74}, byte{0xB7}, byte{0xC4}, byte{0x9F}, byte{0x72},
    byte{0x7E}, byte{0x15}, byte{0x22}, byte{0x12}, byte{0x58}, byte{0x07}, byte{0x99}, byte{0x34},
    byte{0x6E}, byte{0x50}, byte{0xDE}, byte{0x68}, byte{0x65}, byte{0xBC}, byte{0xDB}, byte{0xF8},
    byte{0xC8}, byte{0xA8}, byte{0x2B}, byte{0x40}, byte{0xDC}, byte{0xFE}, byte{0x32}, byte{0xA4},
    byte{0xCA}, byte{0x10}, byte{0x21}, byte{0xF0}, byte{0xD3}, byte{0x5D}, byte{0x0F}, byte{0x00},
    byte{0x6F}, byte{0x9D}, byte{0x36}, byte{0x42}, byte{0x4A}, byte{0x5E}, byte{0xC1}, byte{0xE0}
};

const vector<byte> Q1 = {
    byte{0x75}, byte{0xF3}, byte{0xC6}, byte{0xF4}, byte{0xDB}, byte{0x7B}, byte{0xFB}, byte{0xC8},
    byte{0x4A}, byte{0xD3}, byte{0xE6}, byte{0x6B}, byte{0x45}, byte{0x7D}, byte{0xE8}, byte{0x4B},
    byte{0xD6}, byte{0x32}, byte{0xD8}, byte{0xFD}, byte{0x37}, byte{0x71}, byte{0xF1}, byte{0xE1},
    byte{0x30}, byte{0x0F}, byte{0xF8}, byte{0x1B}, byte{0x87}, byte{0xFA}, byte{0x06}, byte{0x3F},
    byte{0x5E}, byte{0xBA}, byte{0xAE}, byte{0x5B}, byte{0x8A}, byte{0x00}, byte{0xBC}, byte{0x9D},
    byte{0x6D}, byte{0xC1}, byte{0xB1}, byte{0x0E}, byte{0x80}, byte{0x5D}, byte{0xD2}, byte{0xD5},
    byte{0xA0}, byte{0x84}, byte{0x07}, byte{0x14}, byte{0xB5}, byte{0x90}, byte{0x2C}, byte{0xA3},
    byte{0xB2}, byte{0x73}, byte{0x4C}, byte{0x54}, byte{0x92}, byte{0x74}, byte{0x36}, byte{0x51},
    byte{0x38}, byte{0xB0}, byte{0xBD}, byte{0x5A}, byte{0xFC}, byte{0x60}, byte{0x62}, byte{0x96},
    byte{0x6C}, byte{0x42}, byte{0xF7}, byte{0x10}, byte{0x7C}, byte{0x28}, byte{0x27}, byte{0x8C},
    byte{0x13}, byte{0x95}, byte{0x9C}, byte{0xC7}, byte{0x24}, byte{0x46}, byte{0x3B}, byte{0x70},
    byte{0xCA}, byte{0xE3}, byte{0x85}, byte{0xCB}, byte{0x11}, byte{0xD0}, byte{0x93}, byte{0xB8},
    byte{0xA6}, byte{0x83}, byte{0x20}, byte{0xFF}, byte{0x9F}, byte{0x77}, byte{0xC3}, byte{0xCC},
    byte{0x03}, byte{0x6F}, byte{0x08}, byte{0xBF}, byte{0x40}, byte{0xE7}, byte{0x2B}, byte{0xE2},
    byte{0x79}, byte{0x0C}, byte{0xAA}, byte{0x82}, byte{0x41}, byte{0x3A}, byte{0xEA}, byte{0xB9},
    byte{0xE4}, byte{0x9A}, byte{0xA4}, byte{0x97}, byte{0x7E}, byte{0xDA}, byte{0x7A}, byte{0x17},
    byte{0x66}, byte{0x94}, byte{0xA1}, byte{0x1D}, byte{0x3D}, byte{0xF0}, byte{0xDE}, byte{0xB3},
    byte{0x0B}, byte{0x72}, byte{0xA7}, byte{0x1C}, byte{0xEF}, byte{0xD1}, byte{0x53}, byte{0x3E},
    byte{0x8F}, byte{0x33}, byte{0x26}, byte{0x5F}, byte{0xEC}, byte{0x76}, byte{0x2A}, byte{0x49},
    byte{0x81}, byte{0x88}, byte{0xEE}, byte{0x21}, byte{0xC4}, byte{0x1A}, byte{0xEB}, byte{0xD9},
    byte{0xC5}, byte{0x39}, byte{0x99}, byte{0xCD}, byte{0xAD}, byte{0x31}, byte{0x8B}, byte{0x01},
    byte{0x18}, byte{0x23}, byte{0xDD}, byte{0x1F}, byte{0x4E}, byte{0x2D}, byte{0xF9}, byte{0x48},
    byte{0x4F}, byte{0xF2}, byte{0x65}, byte{0x8E}, byte{0x78}, byte{0x5C}, byte{0x58}, byte{0x19},
    byte{0x8D}, byte{0xE5}, byte{0x98}, byte{0x57}, byte{0x67}, byte{0x7F}, byte{0x05}, byte{0x64},
    byte{0xAF}, byte{0x63}, byte{0xB6}, byte{0xFE}, byte{0xF5}, byte{0xB7}, byte{0x3C}, byte{0xA5},
    byte{0xCE}, byte{0xE9}, byte{0x68}, byte{0x44}, byte{0xE0}, byte{0x4D}, byte{0x43}, byte{0x69},
    byte{0x29}, byte{0x2E}, byte{0xAC}, byte{0x15}, byte{0x59}, byte{0xA8}, byte{0x0A}, byte{0x9E},
    byte{0x6E}, byte{0x47}, byte{0xDF}, byte{0x34}, byte{0x35}, byte{0x6A}, byte{0xCF}, byte{0xDC},
    byte{0x22}, byte{0xC9}, byte{0xC0}, byte{0x9B}, byte{0x89}, byte{0xD4}, byte{0xED}, byte{0xAB},
    byte{0x12}, byte{0xA2}, byte{0x0D}, byte{0x52}, byte{0xBB}, byte{0x02}, byte{0x2F}, byte{0xA9},
    byte{0xD7}, byte{0x61}, byte{0x1E}, byte{0xB4}, byte{0x50}, byte{0x04}, byte{0xF6}, byte{0xC2},
    byte{0x16}, byte{0x25}, byte{0x86}, byte{0x56}, byte{0x55}, byte{0x09}, byte{0xBE}, byte{0x91}
};
const vector<vector<byte>> RS = {
    {byte{0x01}, byte{0xA4}, byte{0x55}, byte{0x87}, byte{0x5A}, byte{0x58}, byte{0xDB}, byte{0x9E}},
    {byte{0xA4}, byte{0x56}, byte{0x82}, byte{0xF3}, byte{0x1E}, byte{0xC6}, byte{0x68}, byte{0xE5}},
    {byte{0x02}, byte{0xA1}, byte{0xFC}, byte{0xC1}, byte{0x47}, byte{0xAE}, byte{0x3D}, byte{0x19}},
    {byte{0xA4}, byte{0x55}, byte{0x87}, byte{0x5A}, byte{0x58}, byte{0xDB}, byte{0x9E}, byte{0x03}}
};


uint32_t MDS0(byte b) {
    const vector<byte> M0 = {byte{0x01}, byte{0xEF}, byte{0x5B}, byte{0x5B}, byte{0xC6}, byte{0xC6}, byte{0xD8}, byte{0xD8}};
    uint32_t r = 0;
    for (int i = 0; i < 8; i++) {
        if (to_integer<uint8_t>(b) & (1 << i)) r ^= to_integer<uint8_t>(M0[i]) << (i*4);
    }
    return r;
}

uint32_t MDS1(byte b) {
    const vector<byte> M1 = {byte{0xA4}, byte{0x72}, byte{0x3F}, byte{0x3F}, byte{0x2D}, byte{0x2D}, byte{0xC0}, byte{0xC0}};
    uint32_t r = 0;
    for (int i = 0; i < 8; i++) {
        if (to_integer<uint8_t>(b) & (1 << i)) r ^= to_integer<uint8_t>(M1[i]) << (i*4);
    }
    return r;
}

uint32_t MDS2(byte b) {
    const vector<byte> M2 = {byte{0x52}, byte{0x0E}, byte{0x7F}, byte{0x7F}, byte{0xA4}, byte{0xA4}, byte{0x95}, byte{0x95}};
    uint32_t r = 0;
    for (int i = 0; i < 8; i++) {
        if (to_integer<uint8_t>(b) & (1 << i)) r ^= to_integer<uint8_t>(M2[i]) << (i*4);
    }
    return r;
}

uint32_t MDS3(byte b) {
    const vector<byte> M3 = {byte{0x30}, byte{0x6C}, byte{0xAD}, byte{0xAD}, byte{0x3B}, byte{0x3B}, byte{0x4C}, byte{0x4C}};
    uint32_t r = 0;
    for (int i = 0; i < 8; i++) {
        if (to_integer<uint8_t>(b) & (1 << i)) r ^= to_integer<uint8_t>(M3[i]) << (i*4);
    }
    return r;
}

byte RSMultiply(byte a, byte b) {
    uint8_t r = 0;
    auto aa = to_integer<uint8_t>(a);
    auto bb = to_integer<uint8_t>(b);
    while (bb) {
        if (bb & 1) r ^= aa;
        aa = (aa << 1) ^ (aa & 0x80 ? RS_GF_FDBK : 0);
        bb >>= 1;
    }
    return byte{r};
}

// Генерация S-boxes
void generateSBoxes(vector<uint32_t>& k, vector<vector<uint32_t>>& s, const vector<byte>& key) {
    s.assign(4, vector<uint32_t>(256));
    vector<byte> K(32, byte{0});
    copy(key.begin(), key.begin() + min(key.size(), K.size()), K.begin());
    vector<byte> S(4);
    for (int i = 0; i < 4; ++i) {
        S[i] = byte{0};
        for (int j = 0; j < 8; ++j) {
            int keyIndex = (i * 8 + j) % key.size();
            S[i] ^= RSMultiply(RS[i][j], K[keyIndex]);
        }
    }
    size_t keyBytes = key.size();
    int qRounds = (keyBytes <= 16) ? 2 : (keyBytes <= 24) ? 3 : 4;
    for (int i = 0; i < 256; ++i) {
        byte x = byte{static_cast<uint8_t>(i)};
        for (int r = 0; r < qRounds; ++r) {
            x ^= S[r % 4];
            x = Q0[to_integer<uint8_t>(x)];
            x ^= S[(r + 1) % 4];
            x = Q1[to_integer<uint8_t>(x)];
        }
        s[0][i] = MDS0(x);
        s[1][i] = MDS1(x);
        s[2][i] = MDS2(x);
        s[3][i] = MDS3(x);
    }
    vector<uint32_t> Me(4), Mo(4);
    for (int i = 0; i < 4; ++i) {
        Me[i] = (to_integer<uint8_t>(K[2*i]) << 24) |
                (to_integer<uint8_t>(K[2*i+1]) << 16) |
                (to_integer<uint8_t>(K[2*i+8]) << 8) |
                to_integer<uint8_t>(K[2*i+9]);
        Mo[i] = (to_integer<uint8_t>(K[2*i+16]) << 24) |
                (to_integer<uint8_t>(K[2*i+17]) << 16) |
                (to_integer<uint8_t>(K[2*i+24]) << 8) |
                to_integer<uint8_t>(K[2*i+25]);
    }
}

// Функция h
uint32_t h(uint32_t x, const vector<uint32_t>& L, const vector<vector<uint32_t>>& s) {
    vector<byte> b(4);
    b[0] = byte{static_cast<uint8_t>(x >> 24)};
    b[1] = byte{static_cast<uint8_t>(x >> 16)};
    b[2] = byte{static_cast<uint8_t>(x >> 8)};
    b[3] = byte{static_cast<uint8_t>(x)};
    uint32_t y = 0;
    for (int i = 0; i < 4; ++i) {
        byte keyByte = byte{static_cast<uint8_t>(L[i] >> (24 - 8 * i))};
        byte z = b[i] ^ keyByte;
        y ^= s[i][to_integer<uint8_t>(z)];
    }
    return y;
}

// Функция F
void F(uint32_t R0, uint32_t R1, uint32_t& F0, uint32_t& F1, int round, const vector<uint32_t>& subkeys, const vector<vector<uint32_t>>& s) {
    uint32_t T0 = h(R0, vector<uint32_t>(subkeys.begin() + 8, subkeys.begin() + 12), s);
    uint32_t T1 = h(R1, vector<uint32_t>(subkeys.begin() + 10, subkeys.begin() + 14), s);
    F0 = T0 + T1 + subkeys[2*round + 8];
    F1 = T0 + 2*T1 + subkeys[2*round + 9];
}

// Функция block
void encryptBlock(vector<byte>& block, const vector<uint32_t>& subkeys, const vector<vector<uint32_t>>& s) {
    uint32_t R0, R1, R2, R3, F0, F1;
    R0 = (to_integer<uint8_t>(block[0]) << 24) | (to_integer<uint8_t>(block[1]) << 16) | (to_integer<uint8_t>(block[2]) << 8) | to_integer<uint8_t>(block[3]);
    R1 = (to_integer<uint8_t>(block[4]) << 24) | (to_integer<uint8_t>(block[5]) << 16) | (to_integer<uint8_t>(block[6]) << 8) | to_integer<uint8_t>(block[7]);
    R2 = (to_integer<uint8_t>(block[8]) << 24) | (to_integer<uint8_t>(block[9]) << 16) | (to_integer<uint8_t>(block[10]) << 8) | to_integer<uint8_t>(block[11]);
    R3 = (to_integer<uint8_t>(block[12]) << 24) | (to_integer<uint8_t>(block[13]) << 16) | (to_integer<uint8_t>(block[14]) << 8) | to_integer<uint8_t>(block[15]);
    R0 ^= subkeys[0];
    R1 ^= subkeys[1];
    R2 ^= subkeys[2];
    R3 ^= subkeys[3];
    for (int round = 0; round < MAX_ROUNDS; round++) {
        F(R0, R1, F0, F1, round, subkeys, s);
        R2 = (R2 << 1) | (R2 >> 31);
        R2 ^= F0;
        R3 ^= F1;
        R3 = (R3 >> 1) | (R3 << 31);
        swap(R0, R2);
        swap(R1, R3);
    }
    swap(R0, R2);
    swap(R1, R3);
    R0 ^= subkeys[4];
    R1 ^= subkeys[5];
    R2 ^= subkeys[6];
    R3 ^= subkeys[7];
    block[0] = byte{static_cast<uint8_t>(R0 >> 24)}; block[1] = byte{static_cast<uint8_t>(R0 >> 16)}; block[2] = byte{static_cast<uint8_t>(R0 >> 8)}; block[3] = byte{static_cast<uint8_t>(R0)};
    block[4] = byte{static_cast<uint8_t>(R1 >> 24)}; block[5] = byte{static_cast<uint8_t>(R1 >> 16)}; block[6] = byte{static_cast<uint8_t>(R1 >> 8)}; block[7] = byte{static_cast<uint8_t>(R1)};
    block[8] = byte{static_cast<uint8_t>(R2 >> 24)}; block[9] = byte{static_cast<uint8_t>(R2 >> 16)}; block[10] = byte{static_cast<uint8_t>(R2 >> 8)}; block[11] = byte{static_cast<uint8_t>(R2)};
    block[12] = byte{static_cast<uint8_t>(R3 >> 24)}; block[13] = byte{static_cast<uint8_t>(R3 >> 16)}; block[14] = byte{static_cast<uint8_t>(R3 >> 8)}; block[15] = byte{static_cast<uint8_t>(R3)};
}

void keySchedule(const vector<byte>& key, vector<uint32_t>& subkeys, vector<vector<uint32_t>>& s) {
    s.assign(4, vector<uint32_t>(256));
    generateSBoxes(subkeys, s, key);
    vector<uint32_t> Me(4), Mo(4);
    for (int i = 0; i < 4; i++) {
        Me[i] = (to_integer<uint8_t>(key[2*i]) << 24) | (to_integer<uint8_t>(key[2*i+1]) << 16) | (to_integer<uint8_t>(key[2*i+8]) << 8) | to_integer<uint8_t>(key[2*i+9]);
        Mo[i] = (to_integer<uint8_t>(key[2*i+16]) << 24) | (to_integer<uint8_t>(key[2*i+17]) << 16) | (to_integer<uint8_t>(key[2*i+24]) << 8) | to_integer<uint8_t>(key[2*i+25]);
    }
    for (int i = 0; i < 20; i++) {
        uint32_t A = h(2*i*SK_STEP, Me, s);
        uint32_t B = h((2*i+1)*SK_STEP, Mo, s);
        B = (B << 8) | (B >> 24);
        subkeys[2*i] = A + B;
        subkeys[2*i+1] = (A + 2*B) << SK_ROTL | (A + 2*B) >> (32 - SK_ROTL);
    }
}

void encryptCfb(vector<byte>& data, const vector<uint32_t>& subkeys, const vector<vector<uint32_t>>& s, const vector<byte>& iv) {
    vector<byte> feedback = iv;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % BLOCK_SIZE == 0) {
            encryptBlock(feedback, subkeys, s);
        }
        data[i] ^= feedback[i % BLOCK_SIZE];
        feedback[i % BLOCK_SIZE] = data[i];
    }
}

void decryptCfb(vector<byte>& data, const vector<uint32_t>& subkeys, const vector<vector<uint32_t>>& s, const vector<byte>& iv) {
    vector<byte> feedback = iv;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % BLOCK_SIZE == 0) {
            encryptBlock(feedback, subkeys, s);
        }
        byte cipher_byte = data[i];
        data[i] ^= feedback[i % BLOCK_SIZE];
        feedback[i % BLOCK_SIZE] = cipher_byte;
    }
}

vector<byte> stringToBytes(const string& text) {
    vector<byte> bytes(text.size());
    transform(text.begin(), text.end(), bytes.begin(), [](char c) { return byte{static_cast<uint8_t>(c)}; });
    return bytes;
}

string bytesToString(const vector<byte>& bytes) {
    string text(bytes.size(), '\0');
    transform(bytes.begin(), bytes.end(), text.begin(), [](byte b) { return static_cast<char>(to_integer<uint8_t>(b)); });
    return text;
}

extern "C"{
	void twoFishEncryptFile(const vector<byte>& text, const vector<byte>& key, const vector<byte>& iv, vector<byte>& result) {
		vector<uint32_t> subkeys(40);
		vector<vector<uint32_t>> s;
		keySchedule(key, subkeys, s);
		result = text; 
		encryptCfb(result, subkeys, s, iv);
	}

	void twoFishDecryptFile(const vector<byte>& text, const vector<byte>& key, const vector<byte>& iv, vector<byte>& result) {
		vector<uint32_t> subkeys(40);
		vector<vector<uint32_t>> s;
		keySchedule(key, subkeys, s);
		result = text; 
		decryptCfb(result, subkeys, s, iv);
	}

	void twoFishEncryptString(const string& text, const vector<byte>& key, const vector<byte>& iv, string& result) {
		vector<uint32_t> subkeys(40);
		vector<vector<uint32_t>> s;
		keySchedule(key, subkeys, s);
		vector<byte> data = stringToBytes(text);
		encryptCfb(data, subkeys, s, iv);
		result = bytesToString(data);
	}

	void twoFishDecryptString(const string& text, const vector<byte>& key, const vector<byte>& iv, string& result) {
		vector<uint32_t> subkeys(40);
		vector<vector<uint32_t>> s;
		keySchedule(key, subkeys, s);
		vector<byte> data = stringToBytes(text);
		decryptCfb(data, subkeys, s, iv);
		result = bytesToString(data);
	}
}
