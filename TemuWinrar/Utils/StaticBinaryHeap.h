#pragma once

#include <stdexcept>
#include <utility>

//min heap comparator
template <typename T>
struct DefaultLess {
    bool operator()(const T& a, const T& b) const noexcept {
        return a < b;
    }
};

template <typename T, size_t capacity, typename Compare = DefaultLess<T>>
class StaticBinaryHeap {
public:
    StaticBinaryHeap() : size(0), data{} {}

    size_t getSize() const noexcept {
        return size;
    }

    bool empty() const noexcept {
        return size == 0;
    }

    bool full() const noexcept {
        return size == capacity;
    }

    const T& min() const {
        if (empty())
            throw std::runtime_error("Heap is empty");
        return data[0];
    }

    void insert(const T& value) {
        if (full())
            throw std::runtime_error("Heap is full");

        data[size] = value;
        siftUp(size);
        ++size;
    }

    void insert(T&& value) {
        if (full())
            throw std::runtime_error("Heap is full");

        data[size] = std::move(value);
        siftUp(size);
        ++size;
    }

    T extract() {
        if (empty())
            throw std::runtime_error("Heap is empty");

        T result = std::move(data[0]);

        --size;
        if (size > 0) {
            data[0] = std::move(data[size]);
            siftDown(0);
        }

        return result;
    }

    void clear() noexcept {
        size = 0;
    }

private:
    size_t parent(size_t i) const noexcept {
        return (i - 1) / 2;
    }

    size_t left(size_t i) const noexcept {
        return 2 * i + 1;
    }

    size_t right(size_t i) const noexcept {
        return 2 * i + 2;
    }

    void siftUp(size_t i) noexcept {
        while (i > 0) {
            size_t p = parent(i);
            if (!compare(data[i], data[p]))
                break;
            std::swap(data[i], data[p]);
            i = p;
        }
    }

    void siftDown(size_t i) noexcept {
        while (true) {
            size_t l = left(i);
            if (l >= size) break;

            size_t r = right(i);
            size_t m = l;

            if (r < size && compare(data[r], data[l]))
                m = r;

            if (!compare(data[m], data[i]))
                break;

            std::swap(data[i], data[m]);
            i = m;
        }
    }

private:
    T data[capacity];
    size_t size;
    Compare compare;

};