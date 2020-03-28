## Static Compilation

### Installing Dependencies
* LibSSH: statically linking to it for simplicity (for Ubuntu).
```
cd libs/libssh-0.9.3/
mkdir build && cd build
sudo apt install \
  cmake \
  libgcrypt20
cmake -DUNIT_TESTING=ON -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
make
```

* For Debian the packages in the package-repo are self-contained and there's no need to compile and link against the static library.
