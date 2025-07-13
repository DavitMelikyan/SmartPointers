#include <iostream>

template<typename T>
class ControlBlock {
public:
    T* ptr;
    size_t shared_count;
    size_t weak_count;

    ControlBlock(T* p) : ptr(p), shared_count(1), weak_count(0) {}

    ~ControlBlock() {
        ptr = nullptr;
    }
};


template <typename T>
class SharedPtr {
private:
    T* ptr;
    ControlBlock<T>* cblock;
public:
    explicit SharedPtr(T* p = nullptr) : ptr{p} {
        if (ptr) {
            cblock = new ControlBlock(p);
        }
    }

    SharedPtr(const SharedPtr& other) : ptr{other.ptr}, cblock{other.cblock} {
        if (ptr) ++cblock->shared_count;
    }

    SharedPtr(SharedPtr&& other) noexcept : ptr{other.ptr}, cblock{other.cblock} {
        if (ptr) ++cblock->shared_count;
        other.release();
    }

    template <typename U>
    SharedPtr(std::unique_ptr<U>&& other) {
        ptr = other.release();
        if (ptr) {
            cblock = new ControlBlock(ptr);
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        release();
        ptr = other.ptr;
        cblock = other.cblock;
        ++cblock->shared_count;
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        release();
        ptr = other.ptr;
        cblock = other.cblock;
        other.ptr = nullptr;
        other.cblock = nullptr;
        return *this;
    }

    ~SharedPtr() {
        release();
    }

    void reset(T* new_data) {
        release();
        ptr = new_data;
        if (ptr) {
            cblock = new ControlBlock(ptr);
        }
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(cblock, other.cblock);
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

    int use_count() const { 
        return cblock->shared_count; 
    }

    bool unique() const { 
        return use_count() == 1; 
    }

    explicit operator bool() const noexcept { 
        return ptr != nullptr; 
    }

    bool operator==(const SharedPtr& other) const {
        return ptr == other.ptr;
    }
private:
    void release() {
        if (ptr) {
            if(--(cblock->shared_count) == 0) {
                delete ptr;
            }
            if (cblock->weak_count == 0) {
                delete cblock;
            }
        }
        ptr = nullptr;
        cblock = nullptr;
    }
};

int main() {
    SharedPtr<int> ptr1(new int(10));
    std::cout << *ptr1 << std::endl;
    std::cout << "count: " << ptr1.use_count() << std::endl;
    
    auto ptr2 = std::move(ptr1);
    std::cout << "count: " << ptr2.use_count() << std::endl;
    SharedPtr<int> ptr3(new int(10));
}