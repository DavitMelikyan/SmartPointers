#include "sharedptr.cpp"

template<typename T>
class WeakPtr {
private:
    ControlBlock<T>* cblock;
public:
    WeakPtr() : cblock(nullptr) {}

    WeakPtr(const SharedPtr<T>& shptr) : cblock(shptr.cblock) {
        if (cblock) {
            cblock->weak_count++;
        }
    }

    WeakPtr(const WeakPtr& other) : cblock(other.cblock) {
        if (cblock) {
            cblock->weak_count++;
        }
    }

    ~WeakPtr() {
        release();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }
        release();
        cblock = other.cblock;
        if (cblock) {
            cblock->weak_count++;
        }
        return *this;
    }

    size_t use_count() const {
        return cblock ? cblock->shared_count : 0;
    }

    bool expired() const {
        return use_count() == 0;
    }

    SharedPtr<T> lock() const {
        if (!expired()) {
            SharedPtr<T> shptr;
            shptr.ptr = cblock->ptr;
            shptr.cblock = cblock;
            cblock->shared_count++;
            return shptr;
        }
        return SharedPtr<T>();
    }

private:
    void release() {
        if (cblock) {
            cblock->weak_count--;
            if (cblock->shared_count == 0 && cblock->weak_count == 0) {
                delete cblock;
            }
        }
    }
};
