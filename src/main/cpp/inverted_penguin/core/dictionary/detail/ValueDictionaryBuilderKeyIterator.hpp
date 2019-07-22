#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERKEYITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERKEYITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBuilderBaseIterator.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Builder>
	class ValueDictionaryBuilderKeyIterator :
	    public ValueDictionaryBuilderBaseIterator<
	        Builder, ValueDictionaryBuilderKeyIterator<Builder>
	    > {
	private:
	  typedef ValueDictionaryBuilderBaseIterator<
	      Builder, ValueDictionaryBuilderKeyIterator<Builder>
	  > ParentType;
	  
	public:
	  typedef ValueDictionaryKey value_type;
	  typedef value_type reference;

	public:
	  ValueDictionaryBuilderKeyIterator():
	     ValueDictionaryBuilderKeyIterator(nullptr, 0) {
	  }
	  
	  ValueDictionaryBuilderKeyIterator(Builder* builder, uint32_t index):
	      ParentType(builder, index) {
	  }
	  
	  ValueDictionaryBuilderKeyIterator(
	      const ValueDictionaryBuilderKeyIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderKeyIterator(
	       const ValueDictionaryBuilderBaseIterator<
	           Builder, OtherIterator
	       >& other):
	      ParentType(other) {
	  }

	  reference operator*() const {
	    return this->builder_->keyAt(this->index_);
	  }

	  ValueDictionaryBuilderKeyIterator& operator=(
	      const ValueDictionaryBuilderKeyIterator&
	  ) = default;

	  template <typename OtherIterator>
	  ValueDictionaryBuilderKeyIterator& operator=(
	      const ValueDictionaryBuilderBaseIterator<
	          Builder, OtherIterator
	      >& other) {
	    ParentType::operator=(other);
	    return *this;
	  }
	};

      }
    }
  }
}

#endif

