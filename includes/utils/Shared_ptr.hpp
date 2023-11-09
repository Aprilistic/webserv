#ifndef HTTP_SHARED_PTR_HPP
#define HTTP_SHARED_PTR_HPP

template <typename T> class SharedPtr {
private:
  T *ptr;
  int *refCount;

  void dispose() {
    if (--(*refCount) == 0) {
      delete ptr;
      ptr = NULL; // Avoid dangling pointer in case of exceptions
      delete refCount;
    }
  }

public:
  SharedPtr() : ptr(NULL), refCount(new int(0)) {}

  SharedPtr(T *p) : ptr(p), refCount(p ? new int(1) : new int(0)) {}

  SharedPtr(const SharedPtr &other) : ptr(other.ptr), refCount(other.refCount) {
    if (ptr) {
      (*refCount)++;
    }
  }

  ~SharedPtr() { dispose(); }

  SharedPtr &operator=(SharedPtr other) {
    swap(*this, other);
    return *this;
  }

  friend void swap(SharedPtr &first, SharedPtr &second) {
    std::swap(first.ptr, second.ptr);
    std::swap(first.refCount, second.refCount);
  }

  T &operator*() const { return *ptr; }

  T *operator->() const { return ptr; }

  void reset() {
    dispose();
    ptr = NULL;
    refCount = new int(0);
  }

  void reset(T *p) {
    if (ptr != p) {
      dispose();
      ptr = p;
      refCount = p ? new int(1) : new int(0);
    }
  }
};

#endif
