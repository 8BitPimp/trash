#include "gc.h"

struct object_t : public gc_base_t {
  object_t() : child_(nullptr) {}
  virtual ~object_t(){};
  object_t *child_;

protected:
  void _enumerate(gc_enum_t &func) override {
    func(child_);
  }
};

int main(int argc, char **args) {
  {
    gc_t gc;
    // simple alloc chain
    object_t *obj1 = gc.alloc<object_t>();
    obj1->child_ = gc.alloc<object_t>();
    // lost allocation chain
    object_t *obj2 = gc.alloc<object_t>();
    obj2->child_ = gc.alloc<object_t>();
    obj2->child_->child_ = gc.alloc<object_t>();
    obj2 = nullptr;
    // cyclic reference
    object_t *obj3 = gc.alloc<object_t>();
    obj3->child_ = gc.alloc<object_t>();
    obj3->child_->child_ = obj3;
    // check in discoverable root
    gc.check_in(obj1);
    // collect any unreachable allocations
    gc.collect();
  }
  return 0;
}
