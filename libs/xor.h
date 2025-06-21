#ifndef XOR_H
#define XOR_H

void xorEncryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, std::vector<std::byte>& result);
void xorDecryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, std::vector<std::byte>& result);
void xorEncryptString(const std::string& text, const std::vector<std::byte>& key, std::string& result);
void xorDecryptString(const std::string& text, const std::vector<std::byte>& key, std::string& result);

#endif //XOR_H
