#pragma once

#include "openxbox/cpu.h"

namespace openxbox {

class KvmCpu : public Cpu {

public:
    KvmCpu();
    ~KvmCpu();

};

}