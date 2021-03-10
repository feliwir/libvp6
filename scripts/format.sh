#!/bin/bash
echo "Formatting code"
find src/ -name *.cpp -or -name *.hpp | xargs clang-format -i
find include/ -name *.cpp -or -name *.hpp | xargs clang-format -i
find tools/ -name *.cpp -or -name *.hpp | xargs clang-format -i