#pragma once

namespace openxbox {

/*!
 * Poller function.
 */
typedef int (*PollFunc)(void *data);

/*!
 * An entry in the pollers list.
 */
struct PollerEntry {
    PollFunc pollFunc;
    void *data;
};

}
