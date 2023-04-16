# Lab 2

## Requirements
### Os: linux 
### Libraries
- libzip
- zipcmp
- zipmerge
- ziptool
- openssl (libcrypto)
- boost::program_options
## Build
Build with cmake
```
mkdir build && cd build #insert your favorite directory name instead of build
cmake ..
make
```

## Usage
Does 3 things: 
- gets sha256 hash of given file
- zips given file
- splits given file by delimiter and creates a directory. Example 
```
./bezopastnost -s --delim ,, myfile 
```
myfile as follows
```
aaaa,,aaa,aaaa,,
```
output must be
```
aaaa
```
```
aaa,aaaa
```
if run under sudo exits. Requires sudo on each operation, if needed, but password required only once (or not at all because entered on previous run)

