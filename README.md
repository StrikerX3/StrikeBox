**If you're looking for a functional Xbox emulator, check out [XQEMU](http://xqemu.com/) or [Cxbx-Reloaded](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded)**

---

# OpenXBOX
Open-Source (Original) Xbox Emulation Project

The current state of this thing is just a tad bit more tangible than vaporware.
Essentially right now it just initializes an x86 system (courtesy of
[HAXM](https://github.com/intel/haxm)) and runs whatever is in ROM, which is
provided by the user.

No networking, no audio, no graphics, no games... yet ;).

The goal is to emulate the Xbox at a low level. The user will have to provide
their own dump of the MCPX and BIOS ROMs from an Xbox machine, as well as the
appropriate game media dump in XISO format or from an extracted directory.

How to Build
------------
OpenXBOX uses [CMake](https://cmake.org/) build files to generate projects for
your preferred development platform. OpenXBOX contains multiple modules, as
described in the [Project Structure](#project-structure) section below.

You will need to specify a CPU module for OpenXBOX to run. The CMake option
`CPU_MODULE` gives you a choice of default modules bundled with OpenXBOX that
can be used for development and release builds.

*(Note: The majority of the recent work has been done on Windows with Visual
Studio, so it's likely that this doesn't work on other platforms. If that's the
case, feel free to fix it and submit a pull request!)*

### macOS
You'll need CMake 2.6 or later and [HAXM](https://software.intel.com/en-us/articles/intel-hardware-accelerated-execution-manager-intel-haxm).
This should work anywhere with a bit of work. 

```
$ brew install cmake
$ mkdir build; cd build
$ cmake .. -DCPU_MODULE=haxm && make
$ ./openxbox executable.xbe
```

### Linux
Linux is currently unsupported. [KVM](https://www.kernel.org/doc/Documentation/virtual/kvm/api.txt)
support is planned.

### Windows
You'll need CMake 3.8 or later, [Visual Studio Community 2017](https://www.visualstudio.com/downloads/)
and [HAXM](https://software.intel.com/en-us/articles/intel-hardware-accelerated-execution-manager-intel-haxm).
```
> mkdir build
> cd build
> cmake -G "Visual Studio 15 2017" .. -DCPU_MODULE=haxm         # for 32-bit builds
> cmake -G "Visual Studio 15 2017 Win64" .. -DCPU_MODULE=haxm   # for 64-bit builds
```
The .sln file will be generated in the build folder, ready to build.

Project Structure
-----------------
OpenXBOX is split into multiple modules:
- `core`: the core of the emulator, providing basic emulation logic and a
module interface for the various pieces of hardware that composes the Xbox.
This is a static library meant to be used by front-end engines.
- `cli`: a command line front-end for OpenXBOX.
- `common`: common code shared across all modules.
- `module-common`: contains common definitions and types for OpenXBOX modules.
- `cpu-module`: defines the interface and basic types for CPU modules.
- `cpu-haxm-module`: CPU module implementation using [HAXM](https://github.com/intel/haxm).
 
Debugging Guest Code
--------------------
The guest can be debugged using the GDB debugger. Once enabled, the emulator
will open a TCP socket upon startup and wait for the GDB debugger to connect.
Once connected, you can examine the CPU state, set breakpoints, single-step
instructions, etc. A sample .gdbinit file is provided with useful GDB default
settings to be loaded when you start GDB in this directory.
