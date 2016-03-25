#ifndef _GY_CONTAINERS_H_
#define _GY_CONTAINERS_H_

template<typename T>
struct gyArray
{
  gyArray();
  ~gyArray();
  typedef T ElementType;
  typedef uint32 IteratorType;

  T& operator[](uint32 i);
  const T& operator[](uint32 i) const;

  T* _data;
  uint32 _size;
  uint32 _capacity;
};


template<typename C>
struct gyIterator
{
  C::IteratorType value;
};

#endif