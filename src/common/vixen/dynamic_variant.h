#pragma once

#include <cstdlib>

namespace vixen {

/*!
 * Stores any type of object.
 *
 * Constructors and destructors are automatically invoked. A block of memory is
 * automatically managed by this object and resized if needed by the new type.
 */
class DynamicVariant {
public:
    DynamicVariant() {}
    ~DynamicVariant() {
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
    T* Allocate(Args&&... args) {
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
        m_dtor = [](DynamicVariant& u) { ((T*)u.m_memory)->~T(); };
        return new(m_memory) T(std::forward<Args>(args)...);
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
    void (*m_dtor)(DynamicVariant&) = nullptr;
};

}
