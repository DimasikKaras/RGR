#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <limits>
#include <dlfcn.h>
#include "file.h"
#include "support.h"

using namespace std;

typedef void (*xorEncryptFileFunction)(const vector<std::byte>& , const vector<std::byte>&, vector<std::byte>&);
typedef void (*xorDecryptFileFunction)(const  vector<std::byte>&, const  vector<std::byte>&,  vector<std::byte>&);
typedef void (*xorEncryptStringFunction)(const  string&, const  vector<std::byte>&,  string&);
typedef void (*xorDecryptStringFunction)(const  string&, const  vector<std::byte>&,  string&);

typedef void (*beaufortEncryptFileFunction)(const vector<std::byte>&, const vector<std::byte>&, vector<std::byte>&);
typedef void (*beaufortDecryptFileFunction)(const vector<std::byte>&, const vector<std::byte>&, vector<std::byte>&);
typedef void (*beaufortEncryptStringFunction)(const string&, const vector<std::byte>&, string&);
typedef void (*beaufortDecryptStringFunction)(const string&, const vector<std::byte>&, string&);

typedef void (*twoFishEncryptFileFunction)(const vector<std::byte>&, const vector<std::byte>&, const vector<std::byte>&, vector<std::byte>&);
typedef void (*twoFishDecryptFileFunction)(const vector<std::byte>&, const vector<std::byte>&, const vector<std::byte>&, vector<std::byte>&);
typedef void (*twoFishEncryptStringFunction)(const string&, const vector<std::byte>&, const vector<std::byte>&, string&);
typedef void (*twoFishDecryptStringFunction)(const string&, const vector<std::byte>&, const vector<std::byte>&, string& );


bool checkXorLib(){
	void* xorLib = dlopen("./libs/xorLib.so", RTLD_LAZY);
	if (!xorLib) {
		cerr << "Ошибка загрузки xorLib.so: " << dlerror << endl;
		return false;
	}

	xorEncryptFileFunction xorEncryptFile = nullptr;
	xorDecryptFileFunction xorDecryptFile = nullptr;
	xorEncryptStringFunction xorEncryptString = nullptr;
	xorDecryptStringFunction xorDecryptString = nullptr;

	if (xorLib) {
		xorEncryptFile = (xorEncryptFileFunction)dlsym(xorLib, "xorEncryptFile");
		xorDecryptFile = (xorDecryptFileFunction)dlsym(xorLib, "xorDecryptFile");
		xorEncryptString = (xorEncryptStringFunction)dlsym(xorLib, "xorEncryptString");
		xorDecryptString = (xorDecryptStringFunction)dlsym(xorLib, "xorDecryptString");

		if (!xorEncryptFile || !xorDecryptFile || !xorEncryptString || !xorDecryptString) {
			cerr << "Ошибка загрузки функций XOR: " << dlerror() << endl;
			dlclose(xorLib);
			xorLib = nullptr;
			return false;
		}

	}
	dlclose(xorLib);
	return true;
}

bool checkBeaufortLib(){
	void* beaufortLib = dlopen("./libs/beaufortLib.so", RTLD_LAZY);
	if (!beaufortLib) {
		cerr << "Ошибка загрузки beaufortLib.so: " << dlerror << endl;
		return false;
	}
	
	beaufortEncryptFileFunction beaufortEncryptFile = nullptr;
	beaufortDecryptFileFunction beaufortDecryptFile = nullptr;
	beaufortEncryptStringFunction beaufortEncryptString = nullptr;
	beaufortDecryptStringFunction beaufortDecryptString = nullptr;

	if (beaufortLib) {
		beaufortEncryptFile = (beaufortEncryptFileFunction)dlsym(beaufortLib, "beaufortEncryptFile");
		beaufortDecryptFile = (beaufortDecryptFileFunction)dlsym(beaufortLib, "beaufortDecryptFile");
		beaufortEncryptString = (beaufortEncryptStringFunction)dlsym(beaufortLib, "beaufortEncryptString");
		beaufortDecryptString = (beaufortDecryptStringFunction)dlsym(beaufortLib, "beaufortDecryptString");

		if (!beaufortEncryptFile || !beaufortDecryptFile || !beaufortEncryptString || !beaufortDecryptString) {
			cerr << "Ошибка загрузки функций BEAUFORT: " << dlerror() << endl;
			dlclose(beaufortLib);
			beaufortLib = nullptr;
			return false;
		}
	}
	dlclose(beaufortLib);
	return true;
}

bool checkTwoFishLib(){
	void* twoFishLib = dlopen("./libs/twoFishLib.so", RTLD_LAZY);
	if (!twoFishLib) {
		cerr << "Ошибка загрузки twoFishLib.so: " << dlerror << endl;
		return false;
	}
	
	twoFishEncryptFileFunction twoFishEncryptFile = nullptr;
	twoFishDecryptFileFunction twoFishDecryptFile = nullptr;
	twoFishEncryptStringFunction twoFishEncryptString = nullptr;
	twoFishDecryptStringFunction twoFishDecryptString = nullptr;

	if (twoFishLib) {
		twoFishEncryptFile = (twoFishEncryptFileFunction)dlsym(twoFishLib, "twoFishEncryptFile");
		twoFishDecryptFile = (twoFishDecryptFileFunction)dlsym(twoFishLib, "twoFishDecryptFile");
		twoFishEncryptString = (twoFishEncryptStringFunction)dlsym(twoFishLib, "twoFishEncryptString");
		twoFishDecryptString = (twoFishDecryptStringFunction)dlsym(twoFishLib, "twoFishDecryptString");

		if (!twoFishEncryptFile || !twoFishDecryptFile || !twoFishEncryptString || !twoFishDecryptString) {
			cerr << "Ошибка загрузки функций TWOFISH: " << dlerror() << endl;
			dlclose(twoFishLib);
			twoFishLib = nullptr;
			return false;
		}
	}
	dlclose(twoFishLib);
	return true;
}

enum class Ciphers {XOR, BEAUFORT, TWOFISH, EXIT, INVALIDE};

Ciphers Menu(const int& commands) {
    switch (commands) {
        case 0: return Ciphers::EXIT;
        case 1: return Ciphers::XOR;
        case 2: return Ciphers::BEAUFORT;
        case 3: return Ciphers::TWOFISH;
        default: return Ciphers::INVALIDE;
    }
}

enum class Script {
    EXIT,
    ALLProcessFile,
    EncryptFileOnly,
    DecryptFileOnly,
    ALLProcessText,
    EncryptTextOnly,
    DecryptTextOnly,
};

Script Action(const int& selection) {
    switch (selection) {
        case 0: return Script::EXIT;
        case 1: return Script::ALLProcessFile;
        case 2: return Script::EncryptFileOnly;
        case 3: return Script::DecryptFileOnly;
        case 4: return Script::ALLProcessText;
        case 5: return Script::EncryptTextOnly;
        case 6: return Script::DecryptTextOnly;
    }
}

void showMainMenu() {
    cout << "\n┌──────────────────────────────┐\n";
    cout << "│     АЛГОРИТМЫ ШИФРОВАНИЯ     │\n";
    cout << "├──────────────────────────────┤\n";
    cout << "├ 1. XOR                       ┤\n";
    cout << "├ 2. Бофор                     ┤\n";
    cout << "├ 3. TWOFISH                   ┤\n";
    cout << "│                              │\n";
    cout << "│ 0. Выход                     │\n";
    cout << "└──────────────────────────────┘\n";
    cout << "Выберите алгоритм (0-3): ";
}

void showScenarioMenu(const string& crypt) {
    cout << "\n┌────────────────────────────────────────────────────────┐\n";
    cout << "│                 СЦЕНАРИИ ШИФРОВАНИЯ " << crypt << "                │\n";
    cout << "├────────────────────────────┬───────────────────────────┤\n";
    cout << "│         Для файлов         │        Для консоли        │\n";
    cout << "├────────────────────────────┼───────────────────────────┤\n";
    cout << "│ 1. Шифрование + дешифр.    │ 4. Шифрование + дешифр.   │\n";
    cout << "│ 2. Только шифрование       │ 5. Только шифрование      │\n";
    cout << "│ 3. Только дешифрование     │ 6. Только дешифрование    │\n";
    cout << "│ 0. Выход                   │                           │\n";
    cout << "└────────────────────────────┴───────────────────────────┘\n";
    cout << "Выберите сценарий (0-6): ";
}

int main() {

    while (true) {
        showMainMenu();
        try {
            int commands;
            cin >> commands;    		
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Вы ввели не число");
            }
            switch (Menu(commands)) {
                case Ciphers::EXIT:
                    return 0;
                case Ciphers::XOR: {
					void* xorLib = dlopen("./libs/xorLib.so", RTLD_LAZY);
					if (!xorLib) {
						cerr << "Ошибка загрузки xorLib.so: " << dlerror() << endl;
						continue;
					}
					xorEncryptFileFunction xorEncryptFile = nullptr;
					xorDecryptFileFunction xorDecryptFile = nullptr;
					xorEncryptStringFunction xorEncryptString = nullptr;
					xorDecryptStringFunction xorDecryptString = nullptr;
					
					xorEncryptFile = (xorEncryptFileFunction)dlsym(xorLib, "xorEncryptFile");
					xorDecryptFile = (xorDecryptFileFunction)dlsym(xorLib, "xorDecryptFile");
					xorEncryptString = (xorEncryptStringFunction)dlsym(xorLib, "xorEncryptString");
					xorDecryptString = (xorDecryptStringFunction)dlsym(xorLib, "xorDecryptString");
                    bool Xor = true;
                    while (Xor) {
                        showScenarioMenu("XOR");
                        try {
                            int selection;
                            cin >> selection;
                            if (cin.fail()) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                throw runtime_error("Вы ввели не число");
                            }
                            switch (Action(selection)) {
                                case Script::EXIT: {
                                    Xor = false;
                                    break;
                                }
                                case Script::ALLProcessFile: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    xorEncryptFile(file, key, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    writeBytesToFile(encrypt);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Теперь дешифруем файл..." << endl;
                                    vector<std::byte> decrypt;
                                    xorDecryptFile(encrypt, key, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptFileOnly: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    xorEncryptFile(file, key, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    writeBytesToFile(encrypt);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptFileOnly: {
                                    auto encryptFile = readFile();
                                    if (encryptFile.empty()) break;
                                    auto hexKey = readHexKeyFromFile();
                                    auto key = hexToVector(hexKey);
                                    cout << "Дешифруем..." << endl;
                                    vector<std::byte> decrypt;
                                    xorDecryptFile(encryptFile, key, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::ALLProcessText: {
                                    string text, encrypt, decrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    cout << "Шифровка..." << endl;
                                    xorEncryptString(text, key, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    cout << "Применяем ключ \"" << hexKey << "\" для дешифровки текста \"" << encrypt << "\"" << endl;
                                    cout << "Дешифровка..." << endl;
                                    xorDecryptString(encrypt, key, decrypt);
                                    cout << "Дешифрованный текст: " << decrypt << endl;
                                    if (text == decrypt) {
                                        cout << "Проверка: дешифровка прошла успешно!" << endl;
                                    } else {
                                        cout << "Ошибка: дешифрованный текст не совпадает с исходным!" << endl;
                                    }
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptTextOnly: {
                                    string text, encrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    cout << "Шифровка..." << endl;
                                    xorEncryptString(text, key, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    cout << "Ключ, который использовался при шифровки текста в hex: " << hexKey << endl;
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptTextOnly: {
                                    string cryptText, hexText, hexKey, deCrypt;
                                    try {
                                        cout << "Введите зашифрованный текст, который нужно дешифровать (в hex): ";
                                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        getline(cin, hexText);
                                        if (hexText.empty()) {
                                            throw invalid_argument("Hex-текст не может быть пустым");
                                        }
                                        cryptText = hexToString(hexText);
                                        cout << "Введите ключ для дешифровки в hex: ";
                                        getline(cin, hexKey);
                                        if (hexKey.empty()) {
                                            throw invalid_argument("Hex-ключ не может быть пустым");
                                        }
                                        auto key = hexToVector(hexKey);
                                        cout << "Дешифровка..." << endl;
                                        xorDecryptString(cryptText, key, deCrypt);
                                        cout << "Дешифрованный текст: " << deCrypt << endl;
                                        cout << "Программа завершила работу" << endl;
                                    } catch (const invalid_argument& e) {
                                        cout << "Ошибка входных данных: " << e.what() << endl;
                                    } catch (const exception& e) {
                                        cout << "Ошибка: " << e.what() << endl;
                                    }
                                    break;
                                }
                            }
                        } catch (exception& e) {
                            cout << "Ошибка: " << e.what() << endl;
                        }
                    }
                    if (xorLib) dlclose(xorLib);
                    break;
                }
				case Ciphers::BEAUFORT: {
						void* beaufortLib = dlopen("./libs/beaufortLib.so", RTLD_LAZY);
						if (!beaufortLib) {
							cerr << "Ошибка загрузки beaufortLib.so: " << dlerror() << endl;
							continue;
						}
						beaufortEncryptFileFunction beaufortEncryptFile = nullptr;
						beaufortDecryptFileFunction beaufortDecryptFile = nullptr;
						beaufortEncryptStringFunction beaufortEncryptString = nullptr;
						beaufortDecryptStringFunction beaufortDecryptString = nullptr;
						
						beaufortEncryptFile = (beaufortEncryptFileFunction)dlsym(beaufortLib, "beaufortEncryptFile");
						beaufortDecryptFile = (beaufortDecryptFileFunction)dlsym(beaufortLib, "beaufortDecryptFile");
						beaufortEncryptString = (beaufortEncryptStringFunction)dlsym(beaufortLib, "beaufortEncryptString");
						beaufortDecryptString = (beaufortDecryptStringFunction)dlsym(beaufortLib, "beaufortDecryptString");
						
						bool Beaufort = true;
						while (Beaufort) {
                        showScenarioMenu("BEAUFORT");
                        try {
                            int selection;
                            cin >> selection;
                            if (cin.fail()) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                throw runtime_error("Вы ввели не число");
                            }
                            switch (Action(selection)) {
                                case Script::EXIT: {
                                    Beaufort = false;
                                    break;
                                }
                                case Script::ALLProcessFile: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    beaufortEncryptFile(file, key, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    writeBytesToFile(encrypt);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Теперь дешифруем файл..." << endl;
                                    vector<std::byte> decrypt;
                                    beaufortDecryptFile(encrypt, key, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptFileOnly: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    beaufortEncryptFile(file, key, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    writeBytesToFile(encrypt);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptFileOnly: {
                                    auto encryptFile = readFile();
                                    if (encryptFile.empty()) break;
                                    auto hexKey = readHexKeyFromFile();
                                    auto key = hexToVector(hexKey);
                                    cout << "Дешифруем..." << endl;
                                    vector<std::byte> decrypt;
                                    beaufortDecryptFile(encryptFile, key, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::ALLProcessText: {
                                    string text, encrypt, decrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    cout << "Шифровка..." << endl;
                                    beaufortEncryptString(text, key, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    cout << "Применяем ключ \"" << hexKey << "\" для дешифровки текста \"" << encrypt << "\"" << endl;
                                    cout << "Дешифровка..." << endl;
                                    beaufortDecryptString(encrypt, key, decrypt);
                                    cout << "Дешифрованный текст: " << decrypt << endl;
                                    if (text == decrypt) {
                                        cout << "Проверка: дешифровка прошла успешно!" << endl;
                                    } else {
                                        cout << "Ошибка: дешифрованный текст не совпадает с исходным!" << endl;
                                    }
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptTextOnly: {
                                    string text, encrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    cout << "Шифровка..." << endl;
                                    beaufortEncryptString(text, key, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    cout << "Ключ, который использовался при шифровки текста в hex: " << hexKey << endl;
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptTextOnly: {
                                    string cryptText, hexText, hexKey, deCrypt;
                                    try {
                                        cout << "Введите зашифрованный текст, который нужно дешифровать (в hex): ";
                                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        getline(cin, hexText);
                                        if (hexText.empty()) {
                                            throw invalid_argument("Hex-текст не может быть пустым");
                                        }
                                        cryptText = hexToString(hexText);
                                        cout << "Введите ключ для дешифровки в hex: ";
                                        getline(cin, hexKey);
                                        if (hexKey.empty()) {
                                            throw invalid_argument("Hex-ключ не может быть пустым");
                                        }
                                        auto key = hexToVector(hexKey);
                                        cout << "Дешифровка..." << endl;
                                        beaufortDecryptString(cryptText, key, deCrypt);
                                        cout << "Дешифрованный текст: " << deCrypt << endl;
                                        cout << "Программа завершила работу" << endl;
                                    } catch (const invalid_argument& e) {
                                        cout << "Ошибка входных данных: " << e.what() << endl;
                                    } catch (const exception& e) {
                                        cout << "Ошибка: " << e.what() << endl;
                                    }
                                    break;
                                }
                            }
                        } catch (exception& e) {
                            cout << "Ошибка: " << e.what() << endl;
                        }
                    }
                    if (beaufortLib) dlclose(beaufortLib);
                    break;
                }
                case Ciphers::TWOFISH: {
                	void* twoFishLib = dlopen("./libs/twoFishLib.so", RTLD_LAZY);
					if (!twoFishLib) {
						cerr << "Ошибка загрузки twoFishLib.so: " << dlerror() << endl;
						continue;
					}
					twoFishEncryptFileFunction twoFishEncryptFile = nullptr;
					twoFishDecryptFileFunction twoFishDecryptFile = nullptr;
					twoFishEncryptStringFunction twoFishEncryptString = nullptr;
					twoFishDecryptStringFunction twoFishDecryptString = nullptr;
					
					twoFishEncryptFile = (twoFishEncryptFileFunction)dlsym(twoFishLib, "twoFishEncryptFile");
					twoFishDecryptFile = (twoFishDecryptFileFunction)dlsym(twoFishLib, "twoFishDecryptFile");
					twoFishEncryptString = (twoFishEncryptStringFunction)dlsym(twoFishLib, "twoFishEncryptString");
					twoFishDecryptString = (twoFishDecryptStringFunction)dlsym(twoFishLib, "twoFishDecryptString");
						
                    bool Twofish = true;
                    while (Twofish) {
                        showScenarioMenu("TWOFISH");
                        try {
                            int selection;
                            cin >> selection;
                            if (cin.fail()) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                throw runtime_error("Вы ввели не число");
                            }
                            switch (Action(selection)) {
                                case Script::EXIT: {
                                    Twofish = false;
                                    break;
                                }
                                case Script::ALLProcessFile: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    auto iv = generateKey(); // Используем generateKey для IV
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    twoFishEncryptFile(file, key, iv, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    vector<std::byte> data_with_iv;
                                    data_with_iv.reserve(16 + encrypt.size());
                                    data_with_iv.insert(data_with_iv.end(), iv.begin(), iv.end());
                                    data_with_iv.insert(data_with_iv.end(), encrypt.begin(), encrypt.end());
                                    writeBytesToFile(data_with_iv);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Теперь дешифруем файл..." << endl;
                                    if (data_with_iv.size() < 16) {
                                        cout << "Ошибка: Зашифрованный файл слишком мал" << endl;
                                        break;
                                    }
                                    vector<std::byte> decrypt_iv(data_with_iv.begin(), data_with_iv.begin() + 16);
                                    vector<std::byte> decrypt_ciphertext(data_with_iv.begin() + 16, data_with_iv.end());
                                    vector<std::byte> decrypt;
                                    twoFishDecryptFile(decrypt_ciphertext, key, decrypt_iv, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    if (decrypt == file) {
                                        cout << "Проверка: дешифровка прошла успешно!" << endl;
                                    } else {
                                        cout << "Ошибка: дешифрованный текст не совпадает с исходным!" << endl;
                                    }
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptFileOnly: {
                                    auto file = readFile();
                                    if (file.empty()) break;
                                    auto key = generateKey();
                                    auto iv = generateKey();
                                    cout << "Шифруем... " << endl;
                                    vector<std::byte> encrypt;
                                    twoFishEncryptFile(file, key, iv, encrypt);
                                    cout << "Файл успешно зашифрован, записываем зашифрованный файл..." << endl;
                                    vector<std::byte> data_with_iv;
                                    data_with_iv.reserve(16 + encrypt.size());
                                    data_with_iv.insert(data_with_iv.end(), iv.begin(), iv.end());
                                    data_with_iv.insert(data_with_iv.end(), encrypt.begin(), encrypt.end());
                                    writeBytesToFile(data_with_iv);
                                    cout << "Теперь запишем ключ..." << endl;
                                    auto hexKey = bytesToHex(key);
                                    saveKeyToFile(hexKey);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptFileOnly: {
                                    auto data_with_iv = readFile();
                                    if (data_with_iv.empty() || data_with_iv.size() < 16) {
                                        cout << "Ошибка: Файл пустой или слишком мал" << endl;
                                        break;
                                    }
                                    vector<std::byte> iv(data_with_iv.begin(), data_with_iv.begin() + 16);
                                    vector<std::byte> ciphertext(data_with_iv.begin() + 16, data_with_iv.end());
                                    auto hexKey = readHexKeyFromFile();
                                    auto key = hexToVector(hexKey);
                                    cout << "Дешифруем..." << endl;
                                    vector<std::byte> decrypt;
                                    twoFishDecryptFile(ciphertext, key, iv, decrypt);
                                    cout << "Файл успешно дешифрован, записываем дешифрованный файл..." << endl;
                                    writeBytesToFile(decrypt);
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::ALLProcessText: {
                                    string text, encrypt, decrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    auto iv = generateKey();
                                    cout << "Шифровка..." << endl;
                                    twoFishEncryptString(text, key, iv, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    auto hexIV = bytesToHex(iv);
                                    cout << "Применяем ключ \"" << hexKey << "\" и IV \"" << hexIV << "\" для дешифровки текста \"" << encrypt << "\"" << endl;
                                    cout << "Дешифровка..." << endl;
                                    twoFishDecryptString(encrypt, key, iv, decrypt);
                                    cout << "Дешифрованный текст: " << decrypt << endl;
                                    if (text == decrypt) {
                                        cout << "Проверка: дешифровка прошла успешно!" << endl;
                                    } else {
                                        cout << "Ошибка: дешифрованный текст не совпадает с исходным!" << endl;
                                    }
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::EncryptTextOnly: {
                                    string text, encrypt;
                                    cout << "Введите текст, который нужно зашифровать: ";
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    getline(cin, text);
                                    auto key = generateKey();
                                    auto iv = generateKey();
                                    cout << "Шифровка..." << endl;
                                    twoFishEncryptString(text, key, iv, encrypt);
                                    cout << "Зашифрованный текст: " << encrypt << endl;
                                    cout << "Зашифрованный текст в hex: " << toHex(encrypt) << endl;
                                    auto hexKey = bytesToHex(key);
                                    auto hexIV = bytesToHex(iv);
                                    cout << "Ключ, который использовался при шифровании текста в hex: " << hexKey << endl;
                                    cout << "IV, который использовался при шифровании текста в hex: " << hexIV << endl;
                                    cout << "Программа завершила работу" << endl;
                                    break;
                                }
                                case Script::DecryptTextOnly: {
                                    string cryptText, hexText, hexKey, hexIV, deCrypt;
                                    try {
                                        cout << "Введите зашифрованный текст, который нужно дешифровать (в hex): ";
                                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        getline(cin, hexText);
                                        if (hexText.empty()) {
                                            throw invalid_argument("Hex-текст не может быть пустым");
                                        }
                                        cryptText = hexToString(hexText);
                                        cout << "Введите ключ для дешифровки в hex: ";
                                        getline(cin, hexKey);
                                        if (hexKey.empty()) {
                                            throw invalid_argument("Hex-ключ не может быть пустым");
                                        }
                                        cout << "Введите IV для дешифровки в hex: ";
                                        getline(cin, hexIV);
                                        if (hexIV.empty()) {
                                            throw invalid_argument("Hex-IV не может быть пустым");
                                        }
                                        auto key = hexToVector(hexKey);
                                        auto iv = hexToVector(hexIV);
                                        cout << "Дешифровка..." << endl;
                                        twoFishDecryptString(cryptText, key, iv, deCrypt);
                                        cout << "Дешифрованный текст: " << deCrypt << endl;
                                        cout << "Программа завершила работу" << endl;
                                    } catch (const invalid_argument& e) {
                                        cout << "Ошибка входных данных: " << e.what() << endl;
                                    } catch (const exception& e) {
                                        cout << "Ошибка: " << e.what() << endl;
                                    }
                                    break;
                                }
                            }
                        } catch (exception& e) {
                            cout << "Ошибка: " << e.what() << endl;
                        }
                    }
                    if (twoFishLib) dlclose(twoFishLib);
                    break;
                }
                case Ciphers::INVALIDE: {
                    throw runtime_error("Ожидается число от 0 до 3");
                }
            }
        } catch (const std::exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }
}
