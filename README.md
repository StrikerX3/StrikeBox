**If you're looking for a functional Xbox emulator, check out [XQEMU](http://xqemu.com/)
or [Cxbx-Reloaded](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded).**

---

# StrikeBox
Original Xbox Emulator

A fork of [OpenXBOX](https://github.com/mborgerson/OpenXBOX) by mborgerson,
with a focus on LLE emulation. HLE kernel was an original goal of OpenXBOX; it
will come later in StrikeBox.

The current state of this thing is just a tad bit more tangible than vaporware.
Essentially right now it just initializes an x86 system (courtesy of one of the
various virtualization platforms supported by
[virt86](https://github.com/StrikerX3/virt86)) and runs whatever is in ROM,
which is provided by the user.

If provided with a particular BIOS ROM and a raw hard disk image containing the
Microsoft Xbox Dashboard, it will load the Dashboard software succesfully. It
can also load games from an XISO, although without graphics, input, audio or
networking nothing is really playable... yet ;)

The initial goal is to emulate the original Xbox at a low level. The user will
have to provide their own dump of the MCPX and BIOS ROMs from an Xbox machine,
as well as the appropriate game media dump in XISO format and a hard disk image
containing the system software.

In the future, StrikeBox will attempt to provide high level emulation of the
kernel in order to sidestep the ROM and dashboard requirements. It is a long
way off, as research on the kernel is still incipient and existing
implementations are incomplete, incorrect or straight up copies of illegally
obtained code.

How to Build
------------
StrikeBox uses [CMake](https://cmake.org/) build files to generate projects for
your preferred development platform. You'll need CMake 3.8 or later and the
[virt86](https://github.com/StrikerX3/virt86) library.

### Windows
To make a [Visual Studio 2017](https://www.visualstudio.com/downloads/)
project:

```
> mkdir build
> cd build
> cmake -G "Visual Studio 15 2017" -A Win32 ..   # for 32-bit builds
> cmake -G "Visual Studio 15 2017" -A x64 ..     # for 64-bit builds
```
The .sln file will be generated in the build folder, ready to build.

### Linux

```
$ sudo apt-get install cmake
$ mkdir build; cd build
$ cmake .. && make
$ cd src/cli
$ ./strikebox-cli -m <path-to-MCPX-ROM> -b <path-to-BIOS-ROM> -d <path-to-XBE> -r [debug|retail]
```

### macOS
macOS is currently unsupported. Feel free to submit a pull request to add
support for this platform!

Debugging Guest Code
--------------------
The guest can be debugged using the GDB debugger. Once enabled, the emulator
will open a TCP socket upon startup and wait for the GDB debugger to connect.
Once connected, you can examine the CPU state, set breakpoints, single-step
instructions, etc. A sample .gdbinit file is provided with useful GDB default
settings to be loaded when you start GDB in this directory.

Alternatively, on Windows, you can perform kernel debugging of the virtual Xbox
by creating a linked pair of virtual null-modem serial ports with [com0com](http://com0com.sourceforge.net/).
Use a Debug BIOS ROM and attach one side of the pair to the first Super I/O
serial port, then connect [WinDbg or KD](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/)
to the other side to begin kernel debugging.
