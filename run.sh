#!/bin/bash

module_name="mymodule"
ko_file="./${module_name}.ko"

# Массив из 5 выражений
expressions=("(sin(p))^2+(cos(p))^2" "-2+3-4+5" "2*(1.5+2.5)" "5/3" "exp(0)")

echo "Cleaning up previous files"
# rm -f временные файлы, если есть

echo "Cleaning up previous builds"
make clean

echo "Building kernel module"
make

if [ ! -f "$ko_file" ]; then
    echo "Module not found. Build failed."
    exit 1
fi

for expr in "${expressions[@]}"; do
    echo "Loading kernel module with expression: $expr"
    sudo insmod "$ko_file" expression="$expr"
    if [ $? -ne 0 ]; then
        echo "Failed to load module with expression: $expr"
        exit 1
    fi

    # Ждём, чтобы модуль обработал выражение
    sleep 2

    echo "Unloading kernel module"
    sudo rmmod "$module_name"
done

echo "Last kernel messages:"
sudo dmesg | tail -n 100

