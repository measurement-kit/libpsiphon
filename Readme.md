# Psiphon

This repo will implement the Psiphon OONI test.

## Build instructions

Ensure you have in the root of this directory a shared library build of
psiphon called `libpsiphontunnel.dylib` and under `vendor/` the corresponding
`libpsiphontunnel.h` file. Official releases can be found at
https://github.com/Psiphon-Labs/psiphon-tunnel-core/releases in
`Psiphon-Client-Library.zip`.

### Unix

```
cmake -GNinja .
ninja -v
ctest -a -j8
```
### Msys2

```
cmake -G'Unix Makefiles' .
make
ctest -a -j8
```

### Windows

Open `CMakeLists.txt` with Visual Studio. Make sure you select the
`x64-Release` build, since it's the only supported build for now.
