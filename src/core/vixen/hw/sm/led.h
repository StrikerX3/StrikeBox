#pragma once

namespace vixen {
namespace LED {

// See http://xboxdevwiki.net/PIC#The_LED

union Sequence {
    struct {
        uint8_t greenPhases : 4;
        uint8_t redPhases : 4;
    };
    uint8_t asUint8;

    Sequence(const uint8_t greenPhases, const uint8_t redPhases) {
        this->greenPhases = greenPhases;
        this->redPhases = redPhases;
    }

    Sequence(const uint8_t& val) {
        asUint8 = val;
    }

    operator uint8_t() {
        return asUint8;
    }

    const char *Name(uint8_t phase) {
        if (phase > 3) {
            return "<invalid phase>";
        }

        uint8_t mask = (1 << phase);
        bool green = (greenPhases & mask);
        bool red = (redPhases & mask);
        if (green && red) {
            return "Orange";
        }
        if (green) {
            return "Green";
        }
        if (red) {
            return "Red";
        }
        return "Off";
    }
};

}
}