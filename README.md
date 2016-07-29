C PiGlow library
================

This is a small library for controlling the PiGlow through C, without having to worry about handling I2C (too much).

Preparation
-----------

After cloning the repository, create two directories in the project root folder (same place as src) called build and lib.
Move to the build directory and create the cmake files.

```
 $ mkdir build lib
 $ cd ./build
 $ cmake ..
```

Build
-----

Navigate to the build directory and run `make`.

```
 $ cd ./build # Assuming you are in the root directory
 $ make
```

Install
-------

Navigate to the build directory and run `sudo make install`. `sudo` is required as the files are installed into the `/usr/local` directory.

```
 $ cd ./build # Assuming you are in the root directory
 $ sudo make install
```

Note that the `sudo make install` command will also build the library.
