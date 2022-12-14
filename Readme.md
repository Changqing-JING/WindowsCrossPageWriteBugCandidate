A demo that write cross page with different memory access lead to hardware level undefined behavior
See "Arm® Architecture Reference Manual" section section K1.2.13

## Run on arm64 windows
```shell
mkdir build_win_arm
cd build_win_arm
cmake -G "Visual Studio 17 2022" -A ARM64 ..
cmake --build . --config Debug
```
Copy following files to a arm64 laptop for exmaple surface pro X, put them in a same folder
build_win_arm Debug\\WindowsSignalHandlerBugCandidate.exe
src\LoopRun.bat

Execute:
```
LoopRun.bat
```
After 1-3min execution, the batch exit with output:
```
buffEnd is 807060504030201
buffEnd is 5a07060504030201
```

## As a comparison, x86_64 windows, x86_64 linux doesn't change the memory
#### Run on x86_64 windows
```shell
mkdir build_win
cd build_win
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Debug
Debug\\WindowsSignalHandlerBugCandidate.exe
```
Observed output:
```shell
buffEnd is 807060504030201
```

#### Run on x86_64 linux
```shell
mkdir build
cd build
cmake ..
cmake --build .
./UnixWriteCrossPage
```
Observed output:
```shell
buffEnd is 807060504030201
```

## But Surface Pro X WSL2 also has this problem.
#### Run on arm64 Linux 
```shell
mkdir build_linux_arm
cd build_linux_arm
cmake -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc ..
cmake --build .
#Run on arm64 device
./UnixWriteCrossPage
#Run with qemu
qemu-aarch64 -L /usr/aarch64-linux-gnu ./UnixWriteCrossPage
```
The bug candiate can also be reproduced by WSL2 on surface pro X
Observed output:
```shell
./LoopRun.sh
buffEnd is 807060504030201
buffEnd is 807060504030201
buffEnd is 5a07060504030201
```