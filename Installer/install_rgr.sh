#!/bin/bash

REPO_URL="https://github.com/DimasikKaras/RGR"
BINARY_NAME="RGRapp"

DESKTOP_DIR="$HOME/Desktop"
[ -d "$HOME/Рабочий стол" ] && DESKTOP_DIR="$HOME/Рабочий стол"

if [ ! -d "$DESKTOP_DIR" ]; then
    echo "❌ Ошибка: не найдена папка рабочего стола!"
    echo "Попробую создать Desktop..."
    mkdir -p "$HOME/Desktop" || {
        echo "❌ Не удалось создать папку рабочего стола"
        exit 1
    }
    DESKTOP_DIR="$HOME/Desktop"
fi

INSTALL_DIR="$DESKTOP_DIR/RGRapp"

# --- Подготовка папки назначения ---
echo "📂 Создаём папку программы..."
rm -rf "$INSTALL_DIR"  
mkdir -p "$INSTALL_DIR/libs" || {
    echo "❌ Не удалось создать папку установки"
    exit 1
}
mkdir -p "$INSTALL_DIR/data" || {
    echo "❌ Не удалось создать папку data"
    exit 1
}

echo "📥 Загружаем исходный код..."
CLONE_DIR=$(mktemp -d)
git clone "$REPO_URL" "$CLONE_DIR" || {
    echo "❌ Ошибка при клонировании репозитория"
    rm -rf "$CLONE_DIR"
    exit 1
}

echo "🛠 Собираем программу..."
cd "$CLONE_DIR"
g++ main.cpp support.cpp file.cpp -o "$BINARY_NAME" -ldl || {
    echo "❌ Ошибка компиляции"
    rm -rf "$CLONE_DIR"
    exit 1
}

# --- Перенос файлов ---
echo "📦 Устанавливаем файлы..."
mv "$BINARY_NAME" "$INSTALL_DIR/"
mv libs/*.so "$INSTALL_DIR/libs/" 2>/dev/null || echo "⚠️ Нет библиотек для перемещения"
mv *.cpp *.h "$INSTALL_DIR/" 2>/dev/null || echo "⚠️ Нет исходников для перемещения"
mv data/*.jpg data/*.mp4 "$INSTALL_DIR/data/" 2>/dev/null || echo "⚠️ Нет исходников для перемещения"

echo "🔗 Настраиваем переменные среды..."
grep -q "LD_LIBRARY_PATH.*$INSTALL_DIR/libs" "$HOME/.bashrc" || \
    echo "export LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:$INSTALL_DIR/libs\"" >> "$HOME/.bashrc"

source "$HOME/.bashrc"

# --- Очистка временных файлов ---
rm -rf "$CLONE_DIR"

echo "🔄 Создаём файл для запуска программы..."
LAUNCHER="$DESKTOP_DIR/RGRrun.sh"
cat > "$LAUNCHER" <<EOF
#!/bin/bash
cd "$INSTALL_DIR" || { 
    echo "❌ Ошибка перехода в папку программы: $INSTALL_DIR"
    exit 1
}
LD_LIBRARY_PATH="$INSTALL_DIR/libs:\$LD_LIBRARY_PATH" ./"$BINARY_NAME" "\$@"
EOF

chmod +x "$LAUNCHER"


# --- Итоговое сообщение ---
echo ""
echo "✅ Установка успешно завершена!"
echo "Программа установлена в: $INSTALL_DIR"
echo ""
echo "Теперь вы можете запускать программу через скрипт, который можно найти по пути: "
echo "   $DESKTOP_DIR/RGRapp.sh"

