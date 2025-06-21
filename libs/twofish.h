#ifndef TWOFISH_H
#define TWOFISH_H

void twoFishEncryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, const std::vector<std::byte>& iv, std::vector<std::byte>& result);
void twoFishDecryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, const std::vector<std::byte>& iv, std::vector<std::byte>& result);
void twoFishEncryptString(const std::string& text, const std::vector<std::byte>& key, const std::vector<std::byte>& iv, std::string& result);
void twoFishDecryptString(const std::string& text, const std::vector<std::byte>& key, const std::vector<std::byte>& iv, std::string& result);

#endif // TWOFISH_H
