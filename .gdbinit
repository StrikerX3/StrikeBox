set architecture i386
set disassembly-flavor intel
target remote 127.0.0.1:9269

# To load symbols for your XBE, use something like this:
# add-symbol-file /path/to/your/main.exe 0x17000

# To debug the GDB protocol uncomment the following line:
# set debug remote 1

# Show disassembly and source code
layout split
