#ifndef HTTP_SHARED_PTR_HPP
#define HTTP_SHARED_PTR_HPP

template <typename T> class SharedPtr {
private:
  T *ptr;
  int *refCount;

public:
  SharedPtr() : ptr(NULL), refCount(new int(0)) {}

  SharedPtr(T *p) : ptr(p), refCount(new int(1)) {}

  SharedPtr(const SharedPtr &other) : ptr(other.ptr), refCount(other.refCount) {
    (*refCount)++;
  }

  ~SharedPtr() {
    if (--(*refCount) == 0) {
      delete ptr;
      delete refCount;
    }
  }

  SharedPtr &operator=(const SharedPtr &other) {
    if (this != &other) {
      if (--(*refCount) == 0) {
        delete ptr;
        delete refCount;
      }
      ptr = other.ptr;
      refCount = other.refCount;
      (*refCount)++;
    }
    return *this;
  }

  T &operator*() const { return *ptr; }

  T *operator->() const { return ptr; }

  void reset() {
    if (--(*refCount) == 0) {
      delete ptr;
      delete refCount;
    }
    ptr = NULL;
    refCount = new int(0);
  }

  void reset(T *p) {
    if (--(*refCount) == 0) {
      delete ptr;
      delete refCount;
    }
    ptr = p;
    refCount = new int(1);
  }
};

#endif