#/bin/bash
make clean
cmake ../src -Wno-deprecated
make all