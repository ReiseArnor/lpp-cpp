# lpp-cpp
Platzi Programming Language interpreter made in C++

# Dependencies
1. CMake 3.9
2. GCC >= 11, Clang >= 13 or MSVC >= 19.29

# Installing dependencies
### For Arch/Manjaro (and their derivatives):
```bash
sudo pacman -S cmake clang
```

### For Ubuntu/Debian (and their derivatives)
```bash
sudo apt install cmake clang-13
```
### For Fedora/CentOS (RedHat based distros and their derivatives)
```bash
sudo dnf install cmake clang
```
### For Mac OSX
You need to install [Xcode](https://developer.apple.com/xcode/ "Xcode")
[Installing CMake in OSX](https://macappstore.org/cmake/ "CMake")
### For Windows
You need to install [Visual Studio 2017](https://visualstudio.microsoft.com/es/vs/older-downloads/ "Visual Studio 2017") or [Visual Studio 2019](https://visualstudio.microsoft.com/es/vs/whatsnew/ "Visual Studio 2019")

# Release build
To build the project run the following commands from the root directory.
```bash
cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release
```
# Debug build and test suite

To build the project and run the test suite run the following commands from
the root directory.

```bash
cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug
cd build/Debug
ctest -VV
```
# Run the interpreter
Inside the build directory.
```bash
./lpp_interpreter
```

# A sneak peak of the language
```
Bienvenido al Lenguaje de Programación Platzi.
Escribe un oración para comenzar.
>> variable a = 5;
>> variable b = 10;
>> a + b;
15
>> variable mayor_de_edad = procedimiento(edad) {
        si(edad > 18) {
            regresa verdadero;
        } si_no {
            regresa falso;
        }
    };
>> mayor_de_edad(20);
verdadero
>> mayor_de_edad(15);
falso
>> variable sumador = procedimiento(x) {
       regresa procedimiento(y) {
           regresa x + y;
       };
   };
>> variable suma_dos = sumador(2);
>> suma_dos(5);
7
>> variable suma_cinco = sumador(5);
>> suma_cinco(20);
25
>> mayor_de_edad(suma_cinco(20));
verdadero
```
