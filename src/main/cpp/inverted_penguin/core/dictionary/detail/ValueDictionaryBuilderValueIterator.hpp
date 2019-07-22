#ifndef __INVERTEDPENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERVALUEITERATOR_HPP__
#define __INVERTEDPENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBUILDERVALUEITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryBuilderBaseIterator.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderIterator;

	template <typename Value, typename Builder>
	class ValueDictionaryBuilderValueIterator :
	    public ValueDictionaryBuilderBaseIterator<
	        Builder, ValueDictionaryBuilderValueIterator<Value, Builder>
	    > {
	private:
	  typedef ValueDictionaryBuilderBaseIterator<
	      Builder, ValueDictionaryBuilderValueIterator<Value, Builder>
	  > ParentType;

	public:
	  typedef Value value_type;
	  typedef Value& reference;
	  
	public:
	  ValueDictionaryBuilderValueIterator():
	      ValueDictionaryBuilderValueIterator(nullptr, 0) {
	  }
	  
	  ValueDictionaryBuilderValueIterator(Builder* builder, uint32_t index):
	      ParentType(builder, index) {
	  }
	  
	  ValueDictionaryBuilderValueIterator(
	      const ValueDictionaryBuilderValueIterator&
	  ) = default;

	  ValueDictionaryBuilderValueIterator(
	      const ValueDictionaryBuilderIterator<Value, Builder>& other
	  ): ParentType(other) {
	  }

	  reference operator*() const {
	    return this->builder_->valueAt(this->index_);
	  }

	  ValueDictionaryBuilderValueIterator& operator=(
	      const ValueDictionaryBuilderValueIterator&
	  ) = default;

	  ValueDictionaryBuilderValueIterator& operator=(
	      const ValueDictionaryBuilderIterator<Value, Builder>& other
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
