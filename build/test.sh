#/bin/bash
./build.sh
../dynamorio/bin64/drrun -c ./libstrace.so -- cal
