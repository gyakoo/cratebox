#ifndef _GY_ALGO_H_
#define _GY_ALGO_H_
#include <engine/common/containers.h>

namespace gyAlgo
{
  // gyArray
  template<typename T> uint32 size(const gyArray<T>& container);
  template<typename T> void clear(gyArray<T>& container);
  template<typename T> void clearAndDestroy(gyArray<T>& container);
  template<typename T> void resize(gyArray<T>& container);
  template<typename T> bool empty(const gyArray<T>& container);
  template<typename T> void push_back(gyArray<T>& container, const T& elm);
  template<typename T> void pop_back(gyArray<T>& container);
  template<typename T> void reserve(gyArray<T>& container, uint32 newCapacity);
  template<typename T> T& front(gyArray<T>& container);
  template<typename T> const T& front(const gyArray<T>& container);
  template<typename T> T& back(gyArray<T>& container);
  template<typename T> const T& back(gyArray<T>& container);
  template<typename T> gyIterator< gyArray<T> > begin(gyArray<T>& container);
  template<typename T> gyIterator< gyArray<T> > end(gyArray<T>& container);
  template<typename T> gyIterator< gyArray<T> > begin(const gyArray<T>& container);
  template<typename T> gyIterator< gyArray<T> > end(const gyArray<T>& container);
  template<typename T> T& get(gyArray<T>& container, uint32 at);
  template<typename T> const T& get(const gyArray<T>& container, uint32 at);
  template<typename T> T& get(gyArray<T>& container, gyIterator<gyArray<T>>& at);
  template<typename T> const T& get(const gyArray<T>& container, gyIterator<gyArray<T>>& at);  

  // Algorithms
  template<typename T> gyIterator< gyArray<T> > find( const gyIterator< gyArray<T> >& first, const gyIterator< gyArray<T> >& last, const T& element );
  template<typename T> void sort( gyArray<T>& container );
};

#include <engine/common/algo.inl>

#endif

