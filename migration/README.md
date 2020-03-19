## Static Compilation

### Installing Dependencies
* LibSSH: statically linking to it for simplicity.
```
cd libs/libssh-0.9.3/
mkdir build && cd build
sudo apt install \
  cmake \
  libgcrypt20
cmake -DUNIT_TESTING=ON -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
make
```
