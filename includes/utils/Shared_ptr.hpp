#ifndef HTTP_SHARED_PTR_HPP
#define HTTP_SHARED_PTR_HPP

#include <cstddef>

template <typename T> class SharedPtr {
private:
  T *_ptr;
  size_t *_count;

public:
  SharedPtr(T *ptr = NULL) : _ptr(ptr), _count(new size_t(1)) { (*_count) = 1; }

  SharedPtr(const SharedPtr &orig) : _ptr(orig._ptr), _count(orig._count) {
    (*_count)++;
  }

  SharedPtr &operator=(const SharedPtr &orig) {
    if (this == &orig)
      return (*this);

    (*_count)--;
    if ((*_count) == 0) {
      delete _ptr;
      delete _count;
    }

    _ptr = orig._ptr;
    _count = orig._count;
    (*_count)++;
    return (*this);
  }

  ~SharedPtr() {
    (*_count)--;
    if ((*_count) == 0) {
      delete _ptr;
      delete _count;
    }
  }

  T &operator*(void) { return (*_ptr); }

  const T &operator*(void) const { return (*_ptr); }

  T *operator->(void) { return (_ptr); }

  const T *operator->(void) const { return (_ptr); }
};

#endif
