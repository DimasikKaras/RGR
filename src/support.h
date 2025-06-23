#ifndef SUPPORT_H
#define SUPPORT_H

std::vector<std::byte> generateKey();
std::string bytesToHex(const std::vector<std::byte>& data);
std::vector<std::byte> hexToVector(const std::string& hex);
std::string toHex(const std::string &input);
std::string hexToString(const std::string& hex);


#endif //SUPPORT_H
