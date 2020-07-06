# bench-udp-libs
Benchmarking reliable UDP libraries

# Install
```
# install enet
wget http://enet.bespin.org/enet-1.2.5.tar.gz
tar xvf enet-1.2.5.tar.gz
cd enet-1.2.5
./configure
./configure && make && make install
ldconfig
```

# Result
## ENet by c
```
gcc -o c/enet-bench c/enet-bench.c -lenet
gcc -o c/enet-server c/enet-server.c -lenet
```
