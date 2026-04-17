echo "Building Medusa without external dependencies..."
clang source/main.c source/debug.c source/parser.c source/interpreter.c source/structs.c -O2 -DNDEBUG -MD -fuse-ld=lld -Wall -Wextra -Wpedantic -o build/medusa.exe
echo "Build Complete!"