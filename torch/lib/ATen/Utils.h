#pragma once

#include "ArrayRef.h"
#include "ATenGeneral.h"
#include <sstream>
#include <typeinfo>

namespace at {

#define AT_ASSERT(cond, ...) if (! (cond) ) { at::runtime_error(__VA_ARGS__); }

[[noreturn]]
ATen_CLASS void runtime_error(const char *format, ...);

template <typename T, typename Base>
static inline T* checked_cast(Base* expr, const char * name, int pos, bool allowNull) {
  if(!expr) {
    if (allowNull) {
      return (T*) expr;
    }
    runtime_error("Expected a Tensor of type %s but found an undefined Tensor for argument #%d '%s'",
      T::typeString(),pos,name);
  }
  if (typeid(*expr) != typeid(T))
    runtime_error("Expected object of type %s but found type %s for argument #%d '%s'",
      T::typeString(),expr->type().toString(),pos,name);
  return static_cast<T*>(expr);
}

// Converts a TensorList (i.e. ArrayRef<Tensor> to the underlying TH* Tensor Pointer)
template <typename T, typename TBase, typename TH>
static inline std::vector<TH*> tensor_list_checked_cast(ArrayRef<TBase> tensors, const char * name, int pos) {
  std::vector<TH*> casted(tensors.size());
  for (unsigned int i = 0; i < tensors.size(); ++i) {
    auto *expr = tensors[i].pImpl;
    if (!expr) {
      runtime_error("Expected a Tensor of type %s but found an undefined Tensor for sequence element %u "
                    " in sequence argument at position #%d '%s'",
                    T::typeString(),i,pos,name);
    }
    auto result = dynamic_cast<T*>(expr);
    if (result) {
      casted[i] = result->tensor;
    } else {
      runtime_error("Expected a Tensor of type %s but found a type %s for sequence element %u "
                    " in sequence argument at position #%d '%s'",
                    T::typeString(),expr->type().toString(),i,pos,name);

    }
  }
  return casted;
}

} // at
