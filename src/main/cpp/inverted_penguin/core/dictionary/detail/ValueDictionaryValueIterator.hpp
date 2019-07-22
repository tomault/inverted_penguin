#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYVALUEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYVALUEITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBaseIterator.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class ValueDictionaryValueIterator :
	    public ValueDictionaryBaseIterator<
	        Value, ValueDictionaryValueIterator<Value>
	    > {
	protected:
	  typedef ValueDictionaryBaseIterator<
	      Value, ValueDictionaryValueIterator<Value>
	  > ParentType;
	  using typename ParentType::DictionaryType;

	public:
	  typedef Value value_type;
	  typedef const Value& reference;
	  
	public:
	  ValueDictionaryValueIterator(): ParentType(nullptr, 0) { }
	  
	  ValueDictionaryValueIterator(const DictionaryType* dict,
				       uint32_t index):
	      ParentType(dict, index) {
	  }
	  
	  ValueDictionaryValueIterator(const ValueDictionaryValueIterator&) =
	      default;

	  template <typename OtherIterator>
	  ValueDictionaryValueIterator(
	      const ValueDictionaryBaseIterator<Value, OtherIterator>& other
	  ):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return this->dict_->valueAt(this->index_);
	  }

	  ValueDictionaryValueIterator& operator=(
	      const ValueDictionaryValueIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryValueIterator& operator=(
	      const ValueDictionaryBaseIterator<Value, OtherIterator>& other
	  ) {
	    ParentType::operator=(other);
	    return *this;
	  }
	};
	
      }
    }
  }
}

#endif

