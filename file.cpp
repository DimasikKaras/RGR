#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <limits>
#include <string>
#include <filesystem>
#include "file.h"

using namespace std;


vector<byte> readFile() {
    string filename;
    char isRoot;
    const string dataDir = "data";

    while (true) {
        try {
            while (true) {
                cout << "Файл находится в директории data.? (y/n): ";
                cin >> isRoot;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                isRoot = tolower(isRoot);
                if (isRoot == 'y' || isRoot == 'n') {
                    break;
                }
                cerr << "Ошибка: введите 'y' или 'n'!" << endl;
            }

            if (isRoot == 'n') {
                cout << "Введите путь к файлу: ";
            } else {
                if (!filesystem::exists(dataDir)) {
                    cerr << "Ошибка: каталог '" << dataDir << "' не существует!" << endl;
                    continue; 
                }
                cout << "Введите название файла (из каталога " << dataDir << "): ";
            }

            getline(cin, filename);
            if (filename.empty()) {
                throw runtime_error("Имя файла не может быть пустым!");
            }

            // Формирование полного пути
            string fullPath = (isRoot == 'y') ? (dataDir + "/" + filename) : filename;

            ifstream file(fullPath, ios::binary | ios::ate);
            if (!file.is_open()) {
                throw runtime_error("Не удалось открыть файл '" + fullPath + "'");
            }

            streamsize size = file.tellg();
            if (size <= 0) {
                file.close();
                throw runtime_error("Файл пуст или произошла ошибка определения размера");
            }

            file.seekg(0, ios::beg);
            vector<byte> buffer(size);

            if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
                file.close();
                throw runtime_error("Ошибка чтения данных из файла");
            }

            file.close();
            cout << "Файл '" << fullPath << "' успешно прочитан, размер: " << size << " байт" << endl;
            return buffer;

        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            vector<byte> buffer (0);
            return buffer;
        }
    }
}

void writeBytesToFile(const vector<byte>& data) {
    const string dataDir = "data";
    while (true) {
        try {
            string filename;
            cout << "Введите имя файла для записи: ";
            getline(cin, filename);

            if (filename.empty()) {
                throw runtime_error("Имя файла не может быть пустым!");
            }

            // Создаем директорию data, если её нет
            if (!filesystem::exists(dataDir)) {
                if (!filesystem::create_directory(dataDir)) {
                    throw runtime_error("Не удалось создать директорию '" + dataDir + "'");
                }
                cout << "Директория '" << dataDir << "' создана" << endl;
            }

            // Формируем полный путь к файлу
            string fullPath = dataDir + "/" + filename;

            ofstream file(fullPath, ios::binary);
            if (!file.is_open()) {
                throw runtime_error("Не удалось создать файл '" + fullPath + "'");
            }

            file.write(reinterpret_cast<const char*>(data.data()), data.size());

            if (!file.good()) {
                file.close();
                throw runtime_error("Ошибка записи в файл '" + fullPath + "'");
            }

            file.close();
            cout << "Успешно записано " << data.size() << " байт в файл '" << fullPath << "'" << endl ;
            return;

        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            cerr << "Пожалуйста, попробуйте снова." << endl;
        }
    }
}

void saveKeyToFile(const string& hex_key) {
    string filename;
    bool success = false;
    const string dataDir = "data";

    if (!filesystem::exists(dataDir)) {
        if (!filesystem::create_directory(dataDir)) {
            throw runtime_error("Не удалось создать директорию '" + dataDir + "'");
        }
        cout << "Создана директория '" << dataDir << "'" << endl;
    }

    while (!success) {
        try {
            cout << "Введите имя файла для записи ключа без расширения: ";
            getline(cin, filename);

            if (filename.empty()) {
                throw runtime_error("Имя файла не может быть пустым!");
            }

            // Формируем полный путь к файлу в директории data
            string fullPath = dataDir + "/" + filename + ".txt";

            ofstream file(fullPath);
            if (!file.is_open()) {
                throw runtime_error("Не удалось создать файл '" + fullPath + "'");
            }

            file << hex_key;

            if (!file.good()) {
                throw runtime_error("Ошибка записи в файл '" + fullPath + "'");
            }

            file.close();
            cout << "Ключ успешно записан в файл '" << fullPath << "'" << endl;
            success = true;

        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
        }
    }
}

string readHexKeyFromFile() {
    string filename;
    char isRoot;
    const string dataDir = "data";

    while (true) {
        try {
            while (true) {
                cout << "Файл с ключом находится в директории data/? (y/n): ";
                cin >> isRoot;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                isRoot = tolower(isRoot);
                if (isRoot == 'y' || isRoot == 'n') {
                    break;
                }
                cerr << "Ошибка: введите 'y' или 'n'!" << endl;
            }

            if (isRoot == 'n') {
                cout << "Введите путь к файлу с ключом: ";
            } else {
                // Проверяем существование каталога data
                if (!filesystem::exists(dataDir)) {
                    cerr << "Ошибка: каталог '" << dataDir << "' не существует!" << endl;
                    continue;
                }
                cout << "Введите название файла с ключом (из каталога " << dataDir << "): ";
            }

            getline(cin, filename);
            if (filename.empty()) {
                throw runtime_error("Имя файла не может быть пустым!");
            }

            // Формируем полный путь
            string fullPath = (isRoot == 'y') ? (dataDir + "/" + filename) : filename;

            ifstream file(fullPath);
            if (!file.is_open()) {
                throw runtime_error("Не удалось открыть файл '" + fullPath + "'");
            }

            string hexKey;
            getline(file, hexKey);
            file.close();

            // Проверяем, что ключ не пустой
            if (hexKey.empty()) {
                throw runtime_error("Файл с ключом пуст");
            }

            cout << "Hex-ключ успешно прочитан из файла '" << fullPath << "'" << endl;
            return hexKey;

        } catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            cerr << "Пожалуйста, попробуйте снова." << endl;
        }
    }
}
