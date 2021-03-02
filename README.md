# lpp-cpp
Platzi Programming Language interpreter made in C++

# Dependencies
1. Make
2. CMake
3. GCC >= 5, Clang >= 3.4 or MSVC >= 19.10

# Installing dependencies
### For Arch/Manjaro (and their derivatives):
```bash
sudo pacman -S make cmake gcc clang
```

### For Ubuntu/Debian (and their derivatives)
```bash
sudo apt install build-essential cmake clang-3.4
```
### For Fedora/CentOS (RedHat based distros and their derivatives)
```bash
sudo dnf install make cmake gcc-c++ clang
```
### For Mac OSX
You need to install [Xcode](https://developer.apple.com/xcode/ "Xcode")
### For Windows
You need to install [Visual Studio 2017](https://visualstudio.microsoft.com/es/vs/older-downloads/ "Visual Studio 2017") or [Visual Studio 2019](https://visualstudio.microsoft.com/es/vs/whatsnew/ "Visual Studio 2019")

# Build the project
To build the project run the following commands from the root directory.
#### Linux/OSX
```bash
mkdir build
cmake -Bbuild -H.
cmake --build build/
```
#### Windows
if you have Visual Studio 2017 installed, use "Visual Studio 15 2017".
```bash
mkdir build
cmake -G "Visual Studio 16 2019" -Bbuild
cmake --build build/
```
# Run test suite

To build the project and run the test suite run the following commands from
the root directory.

```bash
mkdir build
cmake -DBUILD_TEST=ON -Bbuild -H.
cmake --build build/
cd build
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
