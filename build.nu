echo "Building Medusa without external dependencies..."
clang main.c parser.c interpreter.c structs.c -O2 -DNDEBUG -MD -fuse-ld=lld -Wall -Wextra -Wpedantic -o build/medusa.exe
echo "Build Complete!"