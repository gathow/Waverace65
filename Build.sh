#!/bin/bash

# Use the directory containing this script as the project directory.
PROJECT_DIR="$(cd -- "$(dirname -- "$0")" && pwd)"
cd "$PROJECT_DIR" || exit 1

echo "Choose build target:"
echo "1) Linux"
echo "2) macOS"
read -r -p "Enter 1 or 2: " choice

rm -f main glad.o
: > compile_error.log

case "$choice" in
  1)
    # Compile GLAD as C, then link it with the C++ app.
    gcc -I include -c src/glad.c -o glad.o 2> compile_error.log
    if [ $? -eq 0 ]; then
      g++ main.cpp glad.o -I include -o main -lglfw -lGL 2>> compile_error.log
    fi
    ;;
  2)
    # MacPorts defaults to /opt/local.
    clang -I include -I/opt/local/include -c src/glad.c -o glad.o 2> compile_error.log
    if [ $? -eq 0 ]; then
      g++ main.cpp glad.o -I include -I/opt/local/include \
        -L/opt/local/lib -lglfw -framework OpenGL -o main 2>> compile_error.log
    fi
    ;;
  *)
    echo "Invalid choice. Enter 1 for Linux or 2 for macOS."
    exit 1
    ;;
esac

if [ $? -eq 0 ]; then
  echo "Build succeeded. Running app..."
  ./main
else
  echo "Compilation failed. Errors:"
  cat compile_error.log
fi
