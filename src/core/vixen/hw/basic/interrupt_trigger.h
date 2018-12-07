#pragma once

namespace vixen {
namespace hw {

class InterruptTrigger {
public:
    virtual void Assert() = 0;
    virtual void Negate() = 0;
};

}
}
