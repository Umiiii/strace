# Strace Equivalent Implementation using DynamoRio

## Introduction

This program is an assigment. It aims to design a `strace` equivalent which outputs exactly the same message as original `strace` without any parameters.

DynamoRio is a dynamic instrumentation tools, which include Dr.Memory framework. 

One of its extensions named Dr.Syscall gives similar functions as `ptrace` does.

Due to the time limitation, only a few of syscall processed.

Only `date` is fully functioned as expected.

## How to Build & Run

Download [DynamoRio](https://github.com/DynamoRIO/dynamorio/wiki/Downloads) and extract into dynamorio folder.
```
cd build
./build.sh
./test.sh
```


