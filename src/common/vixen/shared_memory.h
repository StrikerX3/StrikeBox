#pragma once

#include <cstdlib>

namespace vixen {

/*!
 * A reusable block of memory that dynamically resizes to accomodate the
 * largest data structure written to it.
 *
 * Constructors and destructors are automatically invoked.
 */
class SharedMemory {
public:
    SharedMemory() {}
    ~SharedMemory() {
        if (m_dtor != nullptr) {
            m_dtor(*this);
        }
        if (m_memory != nullptr) {
            free(m_memory);
        }
    }

    /*!
     * Reserves memory for the given data structure, invoking the constructor
     * with the given arguments. Placement new is used to initialize the object
     * or value directly into the allocated memory block.
     *
     * If another data structure is currently occupying the shared memory, it
     * will be destructed before reserving memory for the new data structure.
     *
     * If the new data structure is larger than the currently allocated memory
     * block, a new block will be allocated with the new data structure size,
     * otherwise the existing memory block is reused.
     */
    template<class T, typename... Args>
    T* Allocate(Args... args) {
        // Invoke destructor from previous data structure
        if (m_dtor != nullptr) {
            m_dtor(*this);
        }

        // Reallocate memory if new data structure is larger than the current buffer
        if (m_size < sizeof(T)) {
            if (m_memory != nullptr) {
                free(m_memory);
            }
            m_memory = malloc(sizeof(T));
            m_size = sizeof(T);
        }

        // Update destructor to new data structure and call constructor
        m_dtor = [](SharedMemory& u) { ((T*)u.m_memory)->~T(); };
        return new(m_memory) T(args...);
    }

    /*!
     * Destructs the data structure present in the shared memory, if any.
     */
    void Free() {
        if (m_dtor != nullptr) {
            m_dtor(*this);
            m_dtor = nullptr;
        }
    }

private:
    void *m_memory = nullptr;
    size_t m_size = 0;
    void (*m_dtor)(SharedMemory&) = nullptr;
};

}
