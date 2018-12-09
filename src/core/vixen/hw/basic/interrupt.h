#pragma once

namespace vixen {
namespace hw {

class InterruptTrigger {
public:
    virtual void Assert() = 0;
    virtual void Negate() = 0;
};

class InterruptHook {
public:
    virtual void OnChange(bool asserted) = 0;
};

}
}
