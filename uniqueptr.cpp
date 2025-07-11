#include <iostream>

template<typename T>
class UniquePtr {
private:
    T* ptr;
public:
    explicit UniquePtr(T* p = nullptr) : ptr{p} { }
    ~UniquePtr() {
        delete ptr;
    }
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    UniquePtr(UniquePtr&& other) noexcept : ptr{other.ptr} {
        other.ptr = nullptr;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete ptr;
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T* get() const {
        return ptr;
    }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr, other.ptr);
    }

    operator bool() const {
        return ptr != nullptr;
    }
};

int main() {
    UniquePtr<int> ptr1(new int(10));
    std::cout << *ptr1 << std::endl;
    auto ptr2 = std::move(ptr1);
    return 0;
}