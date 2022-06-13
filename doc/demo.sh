mkdir build
cd build
cmake .. -G Ninja -DCONTRAS_USE_LOGGER=true
cmake --build .
cd ../bin


cd bin
./contras And4.cdl test/And4.in test/And4.out
./contras Or4.cdl test/Or4.in test/Or4.out