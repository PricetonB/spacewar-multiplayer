rm -rf build/*
rm compile_commands.json
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug  -B build
cp ./build/compile_commands.json . 
