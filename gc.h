#pragma once

#include <cassert>
#include <functional>
#include <list>
#include <set>

struct gc_base_t;

using gc_enum_t = std::function<void(gc_base_t *)>;

struct gc_base_t {
protected:
  friend struct gc_t;
  virtual void _enumerate(gc_enum_t &func) = 0;
};

struct gc_t {

  // constructor
  gc_t();

  // release all objects
  ~gc_t();

  // allocate an object
  template <class T, class... Types> T *alloc(Types &&... args) {
    // check allocation derives from object_t
    static_assert(std::is_base_of<gc_base_t, T>::value,
                  "type must derive from object_t");
    // perform the allocation and call constructor
    T *obj = new T(std::forward<Types>(args)...);
    // add to 'allocated' list
    _allocs.push_back(obj);
    return obj;
  }

  // invoke a collection sweep
  void collect();

  // check in a known live object
  void check_in(gc_base_t *);

protected:
  std::list<gc_base_t *> _allocs;
  std::set<gc_base_t *> _valid;
  std::set<gc_base_t *> _mark;
  gc_enum_t _bounce;

  // follow an object and its associated
  void _follow(gc_base_t *obj, std::set<gc_base_t *> &out);
};
