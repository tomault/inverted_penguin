#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEYITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYKEYITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBaseIterator.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class ValueDictionaryKeyIterator :
	    public ValueDictionaryBaseIterator<
	        Value, ValueDictionaryKeyIterator<Value>
	    > {
	protected:
	  typedef ValueDictionaryBaseIterator<
	      Value, ValueDictionaryKeyIterator<Value>
	  > ParentType;
	  using typename ParentType::DictionaryType;

	public:
	  typedef ValueDictionaryKey value_type;
	  typedef value_type reference;
	  
	public:
	  ValueDictionaryKeyIterator(): ParentType(nullptr, 0) { }
	  
	  ValueDictionaryKeyIterator(const DictionaryType* dict,
				     uint32_t index):
	      ParentType(dict, index) {
	  }
	  
	  ValueDictionaryKeyIterator(const ValueDictionaryKeyIterator&) =
	      default;

	  template <typename OtherIterator>
	  ValueDictionaryKeyIterator(
	      const ValueDictionaryBaseIterator<Value, OtherIterator>& other
	  ):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return this->dict_->keyAt(this->index_);
	  }

	  ValueDictionaryKeyIterator& operator=(
	      const ValueDictionaryKeyIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryKeyIterator& operator=(
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

