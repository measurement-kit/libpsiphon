# Psiphon

This repo will implement the Psiphon OONI test.

## Build instructions

Ensure you have in the root of this directory the a shared library build of
psiphon called `psi.dylib`.

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
