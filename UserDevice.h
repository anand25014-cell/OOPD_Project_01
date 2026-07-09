#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H

#include <exception>
//#include <cstring>

class VectorException : public std::exception {
    const char* msg;
public:
    VectorException(const char* m) : msg(m) {}
    const char* what() const noexcept override { return msg; }
};

typedef unsigned int uint;

template<class T>
class GenericVector {
private:
    uint msize;
    uint mcapacity;
    T* data;

public:

    GenericVector() : msize(0), mcapacity(0), data(nullptr) {}

    explicit GenericVector(uint sz) : msize(sz), mcapacity(sz) {
        data = new T[mcapacity];
    }

    GenericVector(const GenericVector &g) : msize(g.msize), mcapacity(g.mcapacity) {
        data = new T[mcapacity];
        for (uint i = 0; i < msize; ++i) data[i] = g.data[i];
    }

    GenericVector& operator=(GenericVector other) {
        std::swap(msize, other.msize);
        std::swap(mcapacity, other.mcapacity);
        std::swap(data, other.data);
        return *this;
    }
    
    T& operator[](uint i) {
        if (i >= msize) throw VectorException("Index out of bounds");
        return data[i];
    }

    const T& operator[](uint i) const {
        if (i >= msize) throw VectorException("Index out of bounds");
        return data[i];
    }


    void push_back(const T& value) {
        if (msize == mcapacity) {
            uint newcap = (mcapacity == 0) ? 1 : mcapacity * 2;
            T* newdata = new T[newcap];
            for (uint i = 0; i < msize; ++i) newdata[i] = data[i];
            delete[] data;
            data = newdata;
            mcapacity = newcap;
        }
        data[msize++] = value;
    }

    void pop_back() {
        if (msize == 0) throw VectorException("pop_back on empty vector");
        msize--;
    }

    uint size() const { return msize; }
    uint capacity() const { return mcapacity; }
    void clear() {
        delete[] data;
        data = nullptr;
        msize = mcapacity = 0;
    }

    ~GenericVector() { delete[] data; }
};

#endif
