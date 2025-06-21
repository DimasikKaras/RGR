#ifndef BEAUFORT_H
#define BEAUFORT_H

void beaufortEncryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, std::vector<std::byte>& result);
void beaufortDecryptFile(const std::vector<std::byte>& text, const std::vector<std::byte>& key, std::vector<std::byte>& result);
void beaufortEncryptString(const std::string& text, const std::vector<std::byte>& key, std::string& result);
void beaufortDecryptString(const std::string& text, const std::vector<std::byte>& key, std::string& result);

#endif //BEAUFORT_H
