#pragma once

#include <stdint.h>

namespace openxbox {

#define BITMASK64(bit) (1ULL << (uint64_t)(bit))

typedef uint64_t Bitmap64;

/*!
 * Determines if a bit is set in the 64-bit bitmap.
 */
inline bool Bitmap64IsSet(Bitmap64 bitmap, uint8_t bit) {
	return (bitmap & BITMASK64(bit)) != 0;
}

/*!
 * Sets a bit in the 64-bit bitmap.
 */
inline void Bitmap64Set(Bitmap64 *bitmap, uint8_t bit) {
	*bitmap |= BITMASK64(bit);
}

/*!
 * Clears a bit in the 64-bit bitmap.
 */
inline void Bitmap64Clear(Bitmap64 *bitmap, uint8_t bit) {
	*bitmap &= ~BITMASK64(bit);
}

}