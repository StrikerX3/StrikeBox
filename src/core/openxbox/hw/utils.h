#pragma once

#include <cstdint>

namespace openxbox {

/*!
 * Get last byte of a range from offset + length.
 * Undefined for ranges that wrap around 0.
 */
static inline uint64_t RangeGetLast(uint64_t offset, uint64_t len) {
    return offset + len - 1;
}

/*!
 * Check whether a given range covers a given byte.
 */
static inline int RangeCoversByte(uint64_t offset, uint64_t len, uint64_t byte) {
    return offset <= byte && byte <= RangeGetLast(offset, len);
}

/*!
 * Check whether 2 given ranges overlap.
 * Undefined if ranges that wrap around 0.
 */
static inline int RangesOverlap(uint64_t first1, uint64_t len1, uint64_t first2, uint64_t len2) {
    uint64_t last1 = RangeGetLast(first1, len1);
    uint64_t last2 = RangeGetLast(first2, len2);

    return !(last2 < first1 || last1 < first2);
}

}
