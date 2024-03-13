#pragma once
#include <algorithm>
#include <sstream>
#include <iostream>
#include <list>
#include <windows.h>
#include <stdlib.h>
#include <cassert>
/*
Memcpy vs. copy_n:
Memcpy:

Функция C, предназначенная для побайтового копирования данных из одного места памяти в другое.
Не зависит от типов данных, что означает, что она может копировать любые типы данных, будь то int, char, float или пользовательские типы.
Быстрая, поскольку работает напрямую с памятью, без использования итераторов.
Небезопасна, если не совпадают размеры источника и назначения, что может привести к переполнению буфера.
Не подходит для копирования сложных типов данных, таких как объекты C++, требующие копирования конструкторов.

copy_n:

Алгоритм STL, предназначенный для копирования элементов из одного диапазона в другой.
Безопасна, поскольку проверяет границы диапазонов и не допускает переполнения буфера.
Универсальная, работает с любыми типами итераторов.
Может быть медленнее memcpy, поскольку использует итераторы для обхода диапазонов.
Подходит для копирования сложных типов данных, так как может использовать конструкторы копирования/перемещения.

Вкратце:

Memcpy: быстрее, но менее безопасна.
copy_n: медленнее, но более безопасна
и подходит для сложных типов данных.

Выбор между memcpy и copy_n:

Если вам нужна максимальная производительность
и вы можете гарантировать, что размеры источника и назначения совпадают,
используйте memcpy.
Если вам нужна безопасность или вы хотите копировать сложные типы данных, используйте copy_n.


ПАРАМЕТРЫ
void* memcpy( void* dest, const void* src, size_t count );
void* dst: Указатель на место назначения копирования.
const void* src: Указатель на источник данных.
size_t n: Количество __байтов__, которые нужно скопировать.


void copy_n (InputIterator first, Size n, OutputIterator result);

OutputIterator dest: Итератор, указывающий на начало места назначения.
InputIterator src: Итератор, указывающий на начало источника.
size_t n: Количество __элементов__, которые нужно скопировать.
4 параметр -- конец источника
*/
using namespace std;
template <typename T>
class VectorLegacy {
private:
    // Размер массива
    size_t m_size;
    // Вместимость массива
    size_t m_capacity;
    // Указатель на массив
    T* m_data;
    // Сортирован ли массив?
    bool sorted;
    /*
    Используем функцию GlobalMemoryStatusEx из Windows API для получения информации о памяти.
    Проверяем, не возникла ли ошибка при получении информации о памяти.
    Возвращаем количество свободной памяти, деля значение ullAvailPhys из структуры MEMORYSTATUSEX на размер типа данных T.
    */
    //Метод для получения размера свободной памяти
    static size_t GetFreeMemory() {
        MEMORYSTATUSEX ms;
        ms.dwLength = sizeof(ms);

        if (!GlobalMemoryStatusEx(&ms)) {
            return 0; // Ошибка при получении информации о памяти
        }

        return ms.ullAvailPhys; // Возвращает количество свободной памяти в указанном типе данных
    }

    // Функция для увеличения вместимости массива
    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        //memcpy(new_data, m_data, m_size * sizeof(T));
        copy_n(m_data, m_size, new_data);
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }



      // Функция для увеличения вместимости массива
    void resize() {
        // Получаем доступ к свободной оперативной памяти. Считаем, сколько еще элементов массива она может вместить в себя
        size_t free_memory = GetFreeMemory()/sizeof(T);

        // Вычисляем 5% от размера оперативной памяти
        size_t memory_limit = free_memory / 100;

        // Увеличиваем вместимость в 2 раза
        size_t new_capacity = m_capacity * 2;

        // Проверяем, превышает ли выделенная память 5% от доступной
        if (new_capacity > memory_limit) {
            // Если превышает, увеличиваем на 1024
            new_capacity = m_capacity + 1024;
        }

        T* new_data = new T[new_capacity];
        //Перенос информации
        copy_n(m_data, m_size, new_data);
        //memcpy(new_data, m_data, m_size * sizeof(T));
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }



      // Функция для перемещения элементов вправо
    void shift_right(size_t index, size_t count) {
        if (m_size + count > m_capacity)
        {
            throw(out_of_range("Not enough capacity to shift"));
        }
    //Не использую memcpy или copy_n во избежание наложения данных друг на друга
        for (size_t i = m_size - 1; i >= index + count; --i) {
            m_data[i + count] = m_data[i];
        }
    }

    // Функция для перемещения элементов влево
    void shift_left(size_t index, size_t count) {
        if (index - count < 0)
        {
            throw(out_of_range("Not enough place to shift"));
        }
        //Не использую memcpy или copy_n во избежание наложения данных друг на друга
        for (size_t i = index; i < m_size - count; ++i) {
            m_data[i] = m_data[i + count];
        }
    }

    size_t partition(size_t low, size_t high) {
        // Выбор опорного элемента
        size_t pivot_index = low + rand() % (high - low + 1);

        T pivot = m_data[pivot_index];
        std::swap(m_data[pivot_index], m_data[high]);

        // Индекс последнего элемента, меньшего опорного
        size_t i = low - 1;

        // Проход по массиву
        for (size_t j = low; j < high; j++) {
            // Если текущий элемент меньше опорного
            if (m_data[j] < pivot) {
                i++;
                std::swap(m_data[i], m_data[j]);
            }
        }

        // Размещение опорного элемента в правильном месте
        std::swap(m_data[i + 1], m_data[high]);

        // Возврат индекса опорного элемента
        return i + 1;
    }

public:
//-----------------------------------ПРАВИЛО ПЯТИ--------------------------------
    // Конструктор по умолчанию
    VectorLegacy() {
        m_size = 0;
        m_capacity = 0;
        m_data = nullptr;
        sorted = false;
    }


    //Конструктор с передачей элементов через список
    template <typename T>
    VectorLegacy(initializer_list<T> list) {
        m_size = list.size();
        m_capacity = m_size;
        m_data = new T[m_size];
        copy_n(list.begin(), m_size, m_data);
        sorted = false;
        //memcpy(m_data, list.begin(), m_size * sizeof(T));
    }


    // Конструктор с указанием размера. Если не указать, каким значением заполнять, заполнится 0
    VectorLegacy(size_t n, const T& value = 0) {
        m_size = n;
        m_capacity = n*2;
        m_data = new T[m_capacity];
        for (size_t i = 0; i < n; ++i) {
            m_data[i] = value;
        }
        sorted = false;
    }



    // Конструктор с указанием элементов из динамического массива
    VectorLegacy(const T* data, size_t n) {
        m_size = n;
        m_capacity = n;
        m_data = new T[n];
        copy_n(data, n, m_data);
        sorted = false;
        //memcpy(m_data, data, n * sizeof(T));
    }

    //Конструктор перемещения
    VectorLegacy(VectorLegacy&& other) noexcept {
        // Перемещение данных
        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        sorted = other.sorted;

        // Обнуление данных other
        other.sorted = false;
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    //Оператор копирования
    VectorLegacy& operator=(const VectorLegacy<T>& other) {
        if (this != &other) {
            // Освобождение памяти
            if (m_data != nullptr) {
                delete[] m_data;
            }

            // Копирование данных
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            sorted = other.sorted;
            m_data = new T[m_capacity];
            //copy_n(other.m_data, other.m_size, m_data, other.m_size);
            memcpy(m_data, other.m_data, other.m_size * sizeof(T));
        }
        return *this;
    }
    //Оператор копирования (списка)
    VectorLegacy& operator=(const initializer_list<T>& list) {
        // Освобождение памяти
        if (m_data != nullptr) {
            delete[] m_data;
        }

        // Копирование данных из списка
        m_size = list.size();
        m_capacity = m_size * 2;
        m_data = new T[m_capacity];
        size_t i = 0;
        copy(list.begin(), list.end(), m_data);
        return *this;
    }

    //Оператор присваивания перемещения
    VectorLegacy& operator=(VectorLegacy<T>&& other) noexcept {
        if (this != &other) {
            // Перемещение данных
            delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            sorted = other.sorted;

            other.sorted = false;
            // Обнуление данных other
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }
    //Конструктор копирования
    VectorLegacy(const VectorLegacy<T>& other) {
        m_size = other.m_size;
        sorted = other.sorted;
        m_capacity = other.m_size;
        m_data = new T[m_capacity];
        //copy_n(other.m_data, other.m_size, m_data, other.m_size);
        memcpy(m_data, other.m_data, m_size * sizeof(T));
    }
    //Обмен массивов местами
    void swap(VectorLegacy& other) {
        if (this == &other) {
            return; // Нечего делать, если это один и тот же объект
        }

        // Сохранение размеров для дальнейшего использования
        size_t this_size = m_size;
        size_t this_capacity = m_capacity;

        // Обмен данными
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
        std::swap(sorted, other.sorted);

        // Обновление ссылок на `nullptr` для объектов, 
        // которые больше не владеют буфером данных
        if (this_size == 0) {
            other.m_data = nullptr;
        }
        if (other.m_size == 0) {
            m_data = nullptr;
        }

        // Обновление capacity для обоих объектов
        if (this_size > this_capacity) {
            other.resize(this_size);
        }
        else if (other.m_size > other.m_capacity) {
            resize(other.m_size);
        }
    }

    // Деструктор
    ~VectorLegacy() {
        if (m_data != nullptr)
        {
        delete[] m_data;
        }
    }
//----------------------------------------------------------------------------------------

    bool operator==(const VectorLegacy<T>& other) const 
    {
        if (m_size != other.m_size) {
            return false;
        }

        for (size_t i = 0; i < m_size; ++i) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }

        return true;
    }

    // Доступ к элементам через []
    T& operator[](size_t index) {
        if (index > m_size)
        {
            throw out_of_range("Tried to access to index out of array size");
        }
        else
        {
            return m_data[index];
        }
    }
    
    const T& operator[](size_t index) const {
        if (index > m_size)
        {
            throw out_of_range("Tried to access to index out of array size");
        }
        else
        {
        return m_data[index];
        }
    }

    // Размер массива
    size_t size() const {
        return m_size;
    }

    // Вместимость массива
    size_t capacity() const {
        return m_capacity;
    }
//----------------------------------------------------------------Добавление и удаление элементов--------------------------------------------------
    // Добавление элемента в конец
    void push_back(const T& value) {
        if (m_size == m_capacity) {
            resize();
        }
        m_data[m_size++] = value;
        sorted = false;
    }

    // Удаление элемента из конца
    T pop_back() {
        T result = m_data[m_size - 1];
        --m_size;
        //Если размер в четыре раза меньше емкости, уменьшаем емкость в 2 раза
        if (m_size <= (m_capacity / 4))
        {
            this->resize(m_capacity/2); 
        }
        return result;
    }

    // Удаление первого элемента
    void pop_front() {
        if (m_size == 0) {
            throw out_of_range("Array is empty");
        }
        shift_left(0, 1);
        --m_size;
    }

    // Добавление элемента в начало
    void push_front(const T& value) {
        if (m_size == m_capacity) {
            resize();
        }

        shift_right(0, 1);
        m_data[0] = value;
        m_size++;
        sorted = false;
    }
    //Вставляет Value в Index
    void insert(size_t index, const T& value) {
        if (index > m_size) {
            throw out_of_range("Index out of range");
        }

        if (m_size == m_capacity) {
            resize();
        }

        shift_right(index, 1);
        m_data[index] = value;
        m_size++;
        sorted = false;
    }
    //Вставляет динамический массив в индекс
    void insert(size_t index, const T* array, size_t count) {
        if (index > m_size) {
            throw out_of_range("Index out of range");
        }

        size_t new_size = m_size + count;
        if (new_size > m_capacity) {
            resize(new_size*2);
        }

        // Сдвиг элементов вправо
        for (size_t i = m_size - 1; i >= index; --i) {
            m_data[i + count] = m_data[i];
        }

        // Копирование данных из array
        //copy_n(array, count, m_data + index, count);
        memcpy(m_data + index, array, count * sizeof(T));
        m_size = new_size;
        sorted = false;
    }
    //Вставляет список в индекс
    void insert(size_t index, const list<T>& list) {
        if (index > m_size) {
            throw out_of_range("Invalid index");
        }

        size_t new_size = m_size + list.size();
        if (new_size > m_capacity) {
            resize(new_size*2);
        }

        // Сдвиг элементов вправо
        for (size_t i = m_size - 1; i >= index; --i) {
            m_data[i + list.size()] = m_data[i];
        }

        // Копирование данных из list
        copy_n(list.begin(), list.size(), m_data + index);
        //memcpy нельзя использовать из-за отсутствия у него в параметрах list
        //memcpy(m_data + index, list.begin(), list.size() * sizeof(T));
        m_size = new_size;
        sorted = false;
    }

    // Очистка массива
    void clear() {
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = 0;
        }
        m_size = 0;
        sorted = false;
    }
    //Удаление элемента массива в индексе
    void delete_(size_t index) {
        if (index >= m_size) {
            throw out_of_range("Invalid index");
        }

        // Сдвиг элементов влево
        for (size_t i = index; i < m_size - 1; ++i) {
            m_data[i] = m_data[i + 1];
        }

        --m_size;
    }
    //Удаление по диапазону
    void delete_(size_t index, size_t count) {
        if (index + count > m_size) {
            throw out_of_range("Invalid index or count");
        }

        // Сдвиг элементов влево
        for (size_t i = index + count; i < m_size; ++i) {
            m_data[i - count] = m_data[i];
        }

        m_size -= count;
    }
//-----------------------------------------------------------------------------------------------------------------------------------
    // Печать элементов
    void print() const {
        //Выводит последний элемент
        for (size_t i = 0; i < m_size; ++i) {
            cout << m_data[i] << " ";
        }
        cout << endl;
    }

    // Конвертация массива в строку
    std::string to_string() const {
        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < m_size; ++i) {
            ss << m_data[i];
            if (i != m_size - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        return ss.str();
    }

    // Доступ к элементу по индексу
    T& at(size_t index) {
        if (index > m_size) 
        {
            throw out_of_range("Tried to access to index out of range (array size)");
        }
        return m_data[index];
        sorted = false;
    }
    // Доступ к элементу по индексу (только чтение)
    const T& at(size_t index) const {
        if (index > m_size) {
            throw out_of_range("Tried to access to index out of range (array size)");
        }
        return m_data[index];
    }
    //Обмен элементов массивов местами
    void swap(size_t index1, size_t index2) {
        if (index1 >= m_size || index2 >= m_size) {
            throw out_of_range("Invalid index");
        }

        // Временная переменная для хранения значения
        T temp = m_data[index1];

        // Обмен значениями элементов
        m_data[index1] = m_data[index2];
        m_data[index2] = temp;
        sorted = false;
    }
    //Поиск value интеополяционно. Сортирует массив по возрастанию, если он не отсортирован
    size_t seek_interpol(const T& value) {
        if (!sorted)
        {

        }

        if (m_size == 0  || m_data[0] > value  || m_data[m_size - 1] < value) {
            return m_size;
        }

        size_t left = 0;
        size_t right = m_size - 1;

        while (left <= right) {
            size_t mid = left + ((right - left) * (value - m_data[left])) / (m_data[right] - m_data[left]);

            if (m_data[mid] == value) {
                return mid;
            }
            else if (m_data[mid] < value) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }

        return m_size;
    }
    //Последовательный поиск
    size_t seek_sequentional(const T& value) const {
        for (size_t i = 0; i < m_size; ++i) {
            if (m_data[i] == value) {
                return i;
            }
        }

        return m_size;
    }
    //Сортировать массив пользователя без спроса -- плохая идея.
    size_t seek(const T& value)
    {
        if (!sorted)
        {
            return seek_sequentional(value);
        }
        else
        {
            return seek_interpol(value);
        }
            
    }

    //Сортировка вставками. Необходима для сортировки
    void sort_insertion(size_t lo, size_t hi) {
        for (size_t i = lo + 1; i < hi; ++i) {
            T value = m_data[i];
            size_t j = i;
            while (j > lo && m_data[j - 1] > value) {
                m_data[j] = m_data[j - 1];
                --j;
            }
            m_data[j] = value;
        }
        sorted = true;
    }

    //Если массив пустой...
    bool empty() const {
        return m_size == 0;
    }
    //Ссылка на последний элемент
    T& back() {
        if (m_size == 0) {
            throw std::out_of_range("Vector is empty");
        }

        return m_data[m_size - 1];
    }
    //Ссылка на первый элемент 
    T& data() {
        if (m_size == 0) {
            throw std::out_of_range("Vector is empty");
        }

        return m_data[0];
    }
    //Указатель на начало массива
    const T* begin() const {
        return m_data;
    }

    //Указатель на конец массива
    const T* end() const {
        return m_data + m_size;
    }

    //iterator begin() {
    //    return iterator(data());
    //}


    //iterator end() {
    //    return iterator(data() + size());
    //}
    //Быстрая сортировка
    void sort_quick(size_t low, size_t high) {
        // Если массив содержит более одного элемента
        if (low < high) {
            // Разбить массив вокруг опорного элемента
            size_t pi = partition(low, high);

            // Рекурсивно отсортировать левый и правый подмассивы
            sort_quick(low, pi);
            sort_quick(pi + 1, high);
        }
        sorted = true;
    }
    //Слияние массивов
    void merge(size_t left, size_t mid, size_t right) {
        // Проверка корректности индексов
        if (left > mid || mid > right) {
            throw std::out_of_range("Invalid indices");
        }

        // Размер временного массива
        size_t temp_size = right - left + 1;

        // Создание временного массива
        VectorLegacy<T> temp(temp_size);

        // Копирование элементов из m_data в temp
        size_t i = left, j = mid + 1, k = 0;
        while (i <= mid && j <= right) {
            if (m_data[i] <= m_data[j]) {
                temp[k++] = m_data[i++];
            }
            else {
                temp[k++] = m_data[j++];
            }
        }

        // Копирование оставшихся элементов из m_data в temp
        while (i <= mid) {
            temp[k++] = m_data[i++];
        }

        while (j <= right) {
            temp[k++] = m_data[j++];
        }

        // Копирование элементов из temp в m_data
        std::copy(temp.begin(), temp.end(), m_data + left);

        // Обновление флага сортировки
        sorted = false;
    }
    //Сортировка слиянеим
    void sort_merge(size_t left, size_t right) {
        if (left < right) {
            size_t mid = (left + right) / 2;
            sort_merge(left, mid);
            sort_merge(mid + 1, right);
            merge(left, mid, right);
        }
        sorted = true;
    }
    //Сортировка. Если меньше миллиона значений, то быстрая, иначе слиянием
    void sort()
    {
        if (size() < 1000000)
        {
            sort_quick(0, size()-1);
        }
        else
        {
            sort_merge(0, size()-1);
        }
        sorted = true;
    }

};

//Процедура тестирования
void test() {
    // Тестирование конструкторов
    VectorLegacy<int> v1;
    assert(v1.size() == 0);
    assert(v1.capacity() == 0);
    assert(v1.empty());

    VectorLegacy<int> v2(5);
    assert(v2.size() == 5);
    assert(v2.capacity() == 10);
    assert(!v2.empty());
    for (int i = 0; i < 5; ++i) {
        assert(v2[i] == 0);
    }

    VectorLegacy<int> v3({ 1, 2, 3, 4, 5 });
    assert(v3.size() == 5);
    assert(v3.capacity() == 5);
    assert(!v3.empty());
    for (int i = 0; i < 5; ++i) {
        assert(v3[i] == i + 1);
    }

    VectorLegacy<int> v4(v3);
    assert(v4.size() == 5);
    assert(v4.capacity() == 5);
    assert(!v4.empty());
    for (int i = 0; i < 5; ++i) {
        assert(v4[i] == i + 1);
    }

    // Тестирование операторов присваивания
    v1 = v2;
    assert(v1.size() == 5);
    assert(v1.capacity() == 10);
    assert(!v1.empty());
    for (int i = 0; i < 5; ++i) {
        assert(v1[i] == 0);
    }

    v1 = { 1, 2, 3, 4, 5 };
    assert(v1.size() == 5);
    assert(v1.capacity() == 10);
    assert(!v1.empty());
    for (int i = 0; i < 5; ++i) {
        assert(v1[i] == i + 1);
    }

    // Тестирование метода push_back
    v1.push_back(6);
    assert(v1.size() == 6);
    assert(v1.capacity() == 10);
    assert(v1[5] == 6);

    // Тестирование метода pop_back
    v1.pop_back();
    assert(v1.size() == 5);
    assert(v1.capacity() == 10);
    assert(v1[4] != 6);

    // Тестирование метода push_front
    v1.push_front(0);
    assert(v1.size() == 6);
    assert(v1.capacity() == 10);
    assert(v1[0] == 0);

    // Тестирование метода pop_front
    v1.pop_front();
    assert(v1.size() == 5);
    assert(v1.capacity() == 10);
    assert(v1[0] != 0);

    // Тестирование метода insert
    v1.insert(2, 10);
    assert(v1.size() == 6);
    assert(v1.capacity() == 10);
    assert(v1[2] == 10);

    // Тестирование метода insert (массив)
    int arr[] = { 11, 12, 13 };
    v1.insert(4, arr, 3);
    assert(v1.size() == 9);
    assert(v1.capacity() == 10);
    assert(v1[4] == 11);
    assert(v1[5] == 12);
    assert(v1[6] == 13);

    // Тестирование метода insert (список)
    list<int> l = { 14, 15, 16 };
    v1.insert(7, l);
    assert(v1.size() == 12);
    assert(v1.capacity() == 24);
    assert(v1[7] == 14);
    assert(v1[8] == 15);
    assert(v1[9] == 16);

    // Тестирование метода clear
    v1.clear();
    assert(v1.size() == 0);
    assert(v1.capacity() == 24);
    assert(v1.empty());

    // Тестирование метода delete_
    v1 = { 1, 2, 3, 4, 5 };
    v1.delete_(2);
    assert(v1.size() == 4);
    assert(v1.capacity() == 10);
    assert(v1[2] == 4);

    // Тестирование метода delete_ (диапазон)
    v1.delete_(1, 2);
    assert(v1.size() == 2);
    assert(v1.capacity() == 10);
    assert(v1[1] == 5);

    // Тестирование метода print
    v1.print(); // Выведет: "1 5"

    // Тестирование метода to_string
    string s = v1.to_string();
    assert(s == "[1, 5]");

    // Тестирование метода at
    assert(v1.at(0) == 1);
    assert(v1.at(1) == 5);

    // Тестирование метода swap
    v1.swap(0, 1);
    assert(v1.at(0) == 5);
    assert(v1.at(1) == 1);

    // Тестирование метода seek_interpol
    v1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    assert(v1.seek_interpol(5) == 4);
    assert(v1.seek_interpol(11) == 10);

    // Тестирование метода seek_sequentional
    assert(v1.seek_sequentional(5) == 4);
    assert(v1.seek_sequentional(11) == 10);

    // Тестирование метода sort_insertion
    v1 = { 5, 3, 1, 2, 4 };
    v1.sort_insertion(0, 5);
    assert(v1 == VectorLegacy<int>({ 1, 2, 3, 4, 5 }));

    // Тестирование метода sort_quick
    v1 = { 5, 3, 1, 2, 4 };
    v1.sort_quick(0, 4);
    assert(v1 == VectorLegacy<int>({ 1, 2, 3, 4, 5 }));

    // Тестирование метода sort_merge
    v1 = { 5, 3, 1, 2, 4 };
    v1.sort_merge(0, 4);
    assert(v1 == VectorLegacy<int>({ 1, 2, 3, 4, 5 }));

    // Тестирование метода sort
    v1 = { 5, 3, 1, 2, 4 };
    v1.sort();
    v1.print();
    assert(v1 == VectorLegacy<int>({ 1, 2, 3, 4, 5 }));

    cout << "All tests passed!" << endl;
}