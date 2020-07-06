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
root:~# gcc -o c/enet-server c/enet-server.c -lenet
root:~# ./c/enet-server
root:~# gcc -o c/enet-bench c/enet-bench.c -lenet
root:~# ./c/enet-bench
Connection to localhost succeeded.
BenchmarkWriteReliablePacket-ENet-C:   14068   1406 loops/s   710 ms/op
```
