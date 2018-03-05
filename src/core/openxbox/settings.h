#pragma once

namespace openxbox {

// FIXME: Make everything here configurable.


// Set to zero to have the system clock interrupt update the system and
// interrupt clocks by a variable high-precision interval so that the clock
// runs in real time.
// Set to non-zero to cause the system clock interrupt to update the system and
// interrupt clocks by the standard increment of 10000. This may cause clock
// drift depending on the performance of the emulation.
#define USE_STANDARD_CLOCK_INCREMENT  0

// Set to zero to use a high-precision timer for KeInterruptTime and
// KeSystemTime.
// Set to non-zero to use KeInterruptTime and KeSystemTime, which are updated
// periodically by the system clock interrupt.
#define USE_SYSTEM_CLOCK_FOR_TIMES  1


// When set to zero, the CPU emulator will dedicate up to 100 ms to execution.
// When non-zero, the CPU emulator will execute one instruction at a time.
#define CPU_SINGLE_STEP  0


// Set to non-zero to enable the GDB server.
#define ENABLE_GDB_SERVER  0


// Set to non-zero to dump XBE section contents.
#define DUMP_SECTION_CONTENTS  0


// Set to non-zero to dump memory address mappings
#define DUMP_MEM_MAPPING  1

}
