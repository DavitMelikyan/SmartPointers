#include <iostream>

template <typename T>
class SharedPtr {
private:
    T* ptr;
    size_t* rcount;
public:
    explicit SharedPtr(T* p = nullptr) : ptr{p}, rcount{p ? new size_t(1) : new size_t(0)} { }

    SharedPtr(const SharedPtr& other) : ptr{other.ptr}, rcount{other.rcount} {
        if (ptr) ++(*rcount);
    }

    SharedPtr(SharedPtr&& other) noexcept : ptr{other.ptr}, rcount{other.rcount} {
        if (ptr) ++(*rcount);
        other.release();
    }

    template <typename U>
    SharedPtr(std::unique_ptr<U>&& other) {
        ptr = other.release();
        rcount = new size_t(ptr ? 1 : 0);
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        release();
        ptr = other.ptr;
        rcount = other.rcount;
        ++(*rcount);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        release();
        ptr = other.ptr;
        rcount = other.rcount;
        other.ptr = nullptr;
        other.rcount = nullptr;
        return *this;
    }

    ~SharedPtr() {
        release();
    }

    void reset(T* new_data) {
        release();
        ptr = new_data;
        rcount = new size_t(new_data ? 1 : 0);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(rcount, other.rcount);
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
        return *rcount; 
    }

    bool unique() const { 
        return use_count() == 1; 
    }

    operator bool() const { 
        return ptr != nullptr; 
    }

    bool operator==(const SharedPtr& other) const {
        return ptr == other.ptr;
    }
private:
    void release() {
        if (ptr) {
            if(--(*rcount) == 0) {
                delete ptr;
                delete rcount;
            }
        }
        ptr = nullptr;
        rcount = nullptr;
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