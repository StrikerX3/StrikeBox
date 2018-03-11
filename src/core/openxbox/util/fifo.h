#pragma once

#include <cstdint>

#include "openxbox/log.h"

namespace openxbox {

template<typename T>
class Fifo {
public:
    Fifo(uint32_t capacity);
    ~Fifo();

    bool Push(T data);
    bool Pop(T *data);
    bool Discard();

    inline void Clear() { m_num = 0; }
    inline uint32_t Count() const { return m_num; }

    inline bool IsEmpty() const { return m_num == 0; }
    inline bool IsFull() const { return m_num == m_capacity; }
private:
    T *m_data;
    uint32_t m_capacity;
    uint32_t m_head;
    uint32_t m_num;
};

template<typename T>
Fifo<T>::Fifo(uint32_t capacity) {
    m_data = new uint8_t[capacity];
    m_capacity = capacity;
    m_head = 0;
    m_num = 0;
}

template<typename T>
Fifo<T>::~Fifo() {
    delete[] m_data;
}

template<typename T>
bool Fifo<T>::Push(T data) {
    if (m_num == m_capacity) {
        log_debug("Fifo::Push: Queue full!\n");
        return false;
    }
    m_data[(m_head + m_num) % m_capacity] = data;
    m_num++;
    return true;
}

template<typename T>
bool Fifo<T>::Pop(T *data) {
    if (m_num == 0) {
        log_debug("Fifo::Pop: Queue empty!\n");
        return false;
    }
    *data = m_data[m_head++];
    m_head %= m_capacity;
    m_num--;
    return true;
}

template<typename T>
bool Fifo<T>::Discard() {
    if (m_num == 0) {
        log_debug("Fifo::Discard: Queue empty!\n");
        return false;
    }
    m_head = (m_head + 1) % m_capacity;
    m_num--;
    return true;
}

}

