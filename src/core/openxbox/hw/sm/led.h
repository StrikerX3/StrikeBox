#pragma once

namespace openxbox {
namespace LED {

// See http://xboxdevwiki.net/PIC#The_LED

enum Color : uint8_t {
    Off = 0b00,
    Green = 0b01,
    Red = 0b10,
    Orange = 0b11,
};

const char *Name(Color color) {
    switch (color) {
    case Off: return "Off";
    case Green: return "Green";
    case Red: return "Red";
    case Orange: return "Orange";
    default: assert(false); return "<invalid>";
    }
}

union Sequence {
    struct {
        Color phase0 : 2;
        Color phase1 : 2;
        Color phase2 : 2;
        Color phase3 : 2;
    };
    uint8_t asUint8;

    Sequence(const Color phase0, const Color phase1, const Color phase2, const Color phase3) {
        this->phase0 = phase0;
        this->phase1 = phase1;
        this->phase2 = phase2;
        this->phase3 = phase3;
    }

    Sequence(const uint8_t& val) {
        asUint8 = val;
    }

    operator uint8_t() {
        return asUint8;
    }
};

}
}