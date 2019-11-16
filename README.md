# Project_Application_Development
This repository implements an emulator capable of executing IJVM byte-code. 



# Compiling
Requires make and GCC or Clang

Run `make ijvm` to build the ijvm binary

You can enable the debug print (`dprintf`) found in `include/util.h` by
setting the `-DDEBUG` compiler flag (e.g., `make clean && make testbasic CFLAGS=-DDEBUG`).

# Running a binary
Run an IJVM program using `./ijvm binary`. For example `./ijvm files/advanced/Tanenbaum.ijvm`.

## Adding header files
Add your header files to the folder `include`.

# Testing
To run a specific test run `make run_testX` (e.g. `make run_test1`).

* To run all basic tests, do `make testbasic`.
* To run all advanced tests, do `make testadvanced`.
* Check for memory leaks using `make testleaks`
* Check for memory errors/ undeifned behavior `make testsanitizers` (requires LLVM)
* To compile with pedantic flags: `make pedantic`

You can debug the tests by running running the binaries generated by
`make build_tests` through GDB.

# Handing in
Generate a gzipped tarball of your project using the `make dist` command.
Make sure to double check that all your required files are included in the tarball.

# Compatibility
You need a valid C11 compiler, such as clang or gcc, as well as glibc. Do not 
use any non-standard libraries.

This skeleton works on Linux and MacOS. Windows users can install a Linux VM, or
follow the instructions below.

# Windows
To develop on Windows, we recommend using the [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10),
and installing Ubuntu from the Microsoft Store. Once you have gone through the
setup steps, you can access the Linux command line by executing `bash` from
the Windows command line. You can access your files in the Windows file system
through mount points (e.g. `cd /mnt/c/Users/<username>/Desktop`).

Both glibc, gcc, and make are included in the package `build-essential`. To 
install this package, execute the following commands:

* `sudo apt-get update`
* `sudo apt-get install build-essential`

Now you can compile the project by navigating to this directory and executing
the `make` command.

# Tools
You can install the goJASM assembler by executing `make tools`. This will
download a goJASM executable in the tools directory.
