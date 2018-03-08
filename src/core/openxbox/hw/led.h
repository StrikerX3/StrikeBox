#pragma once

namespace openxbox {
namespace LED {

// See http://xboxdevwiki.net/PIC#The_LED
namespace Phase0 { enum { Off = 0, Red = 1 << 7, Green = 1 << 3, Orange = Red | Green }; };
namespace Phase1 { enum { Off = 0, Red = 1 << 6, Green = 1 << 2, Orange = Red | Green }; };
namespace Phase2 { enum { Off = 0, Red = 1 << 5, Green = 1 << 1, Orange = Red | Green }; };
namespace Phase3 { enum { Off = 0, Red = 1 << 4, Green = 1 << 0, Orange = Red | Green }; };

typedef uint8_t Sequence;

// Pre-defined LED Phase sequences

constexpr Sequence OFF = Phase0::Off | Phase1::Off | Phase2::Off | Phase3::Off;

// Solid single color: constant on 	        
constexpr Sequence RED = Phase0::Red | Phase1::Red | Phase2::Red | Phase3::Red;
constexpr Sequence GREEN = Phase0::Green | Phase1::Green | Phase2::Green | Phase3::Green;
constexpr Sequence ORANGE = Phase0::Orange | Phase1::Orange | Phase2::Orange | Phase3::Orange;

// Signal single color: thrice on, once off
constexpr Sequence SIGNAL_RED = Phase0::Red | Phase1::Red | Phase2::Red | Phase3::Off;
constexpr Sequence SIGNAL_GREEN = Phase0::Green | Phase1::Green | Phase2::Green | Phase3::Off;
constexpr Sequence SIGNAL_ORANGE = Phase0::Orange | Phase1::Orange | Phase2::Orange | Phase3::Off;

// Blink single color: twice on, twice off
constexpr Sequence BLINK_RED = Phase0::Red | Phase1::Red | Phase2::Off | Phase3::Off;
constexpr Sequence BLINK_GREEN = Phase0::Green | Phase1::Green | Phase2::Off | Phase3::Off;
constexpr Sequence BLINK_ORANGE = Phase0::Orange | Phase1::Orange | Phase2::Off | Phase3::Off;

// Blip single color: once on, thrice off
constexpr Sequence BLIP_RED = Phase0::Red | Phase1::Off | Phase2::Off | Phase3::Off;
constexpr Sequence BLIP_GREEN = Phase0::Green | Phase1::Off | Phase2::Off | Phase3::Off;
constexpr Sequence BLIP_ORANGE = Phase0::Orange | Phase1::Off | Phase2::Off | Phase3::Off;

// Blip twice, single color: on, off, on, off
constexpr Sequence BLIP_RED_RED = Phase0::Red | Phase1::Off | Phase2::Red | Phase3::Off;
constexpr Sequence BLIP_GREEN_GREEN = Phase0::Green | Phase1::Off | Phase2::Green | Phase3::Off;
constexpr Sequence BLIP_ORANGE_ORANGE = Phase0::Orange | Phase1::Off | Phase2::Orange | Phase3::Off;

// Blip, two colors: first, off, second, off
constexpr Sequence BLIP_RED_GREEN = Phase0::Red | Phase1::Off | Phase2::Green | Phase3::Off;
constexpr Sequence BLIP_RED_ORANGE = Phase0::Red | Phase1::Off | Phase2::Orange | Phase3::Off;
constexpr Sequence BLIP_GREEN_ORANGE = Phase0::Green | Phase1::Off | Phase2::Orange | Phase3::Off;

// Solid, two colors: alternating
constexpr Sequence RED_GREEN = Phase0::Red | Phase1::Red | Phase2::Green | Phase3::Green;
constexpr Sequence RED_ORANGE = Phase0::Red | Phase1::Red | Phase2::Orange | Phase3::Orange;
constexpr Sequence GREEN_ORANGE = Phase0::Green | Phase1::Green | Phase2::Orange | Phase3::Orange;

// Solid, two colors: alternating quickly
constexpr Sequence FAST_RED_GREEN = Phase0::Red | Phase1::Green | Phase2::Red | Phase3::Green;
constexpr Sequence FAST_RED_ORANGE = Phase0::Red | Phase1::Orange | Phase2::Red | Phase3::Orange;
constexpr Sequence FAST_GREEN_ORANGE = Phase0::Green | Phase1::Orange | Phase2::Green | Phase3::Orange;

}
}