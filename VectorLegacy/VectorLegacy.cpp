#include "VectorLegacy.h"

template <typename T>
VectorLegacy<T>::VectorLegacy() {
    m_size = 0;
    m_capacity = 0;
    m_data = nullptr;
}

template <typename T>
VectorLegacy<T>::VectorLegacy(size_t n, const T& value) {
    m_size = n;
    m_capacity = n;
    m_data = new T[n];
    for (size_t i = 0; i < n; ++i) {
        m_data[i] = value;
    }
}

template <typename T>
VectorLegacy<T>::VectorLegacy(const T* data, size_t n) {
    m_size = n;
    m_capacity = n;
    m_data = new T[n];
    copy_n(data, n, m_data);
}

template <typename T>
VectorLegacy<T>::~VectorLegacy() {
    delete[] m_data;
}

template <typename T>
T& VectorLegacy<T>::operator[](size_t index) {
    return m_data[index];
}

template <typename T>
const T& VectorLegacy<T>::operator[](size_t index) const {
    return m_data[index];
}

template <typename T>
size_t VectorLegacy<T>::size() const {
    return m_size;
}

template <typename T>
size_t VectorLegacy<T>::capacity() const {
    return m_capacity;
}

template <typename T>
void VectorLegacy<T>::push_back(const T& value) {
    if (m_size == m_capacity) {
        resize(m_capacity * 2);
    }
    m_data[m_size++] = value;
}

template <typename T>
void VectorLegacy<T>::pop_back() {
    --m_size;
}

template <typename T>
void VectorLegacy<T>::clear() {
    m_size = 0;
}

template <typename T>
void VectorLegacy<T>::print() const {
    for (size_t i = 0; i < m_size; ++i) {
        cout << m_data[i] << " ";
    }
    cout << endl;
}

template <typename T>
void VectorLegacy<T>::resize(size_t new_capacity) {
    T* new_data = new T[new_capacity];
    copy_n(m_data, m_size, new_data);
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}
