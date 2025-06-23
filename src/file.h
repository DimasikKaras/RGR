#ifndef FILE_H
#define FILE_H
#include<vector>
#include <cstddef>

std::vector<std::byte> readFile();
void writeBytesToFile(const std::vector<std::byte>& data);
void saveKeyToFile(const std::string& hex_key);
std::string readHexKeyFromFile();
#endif //FILE_H
