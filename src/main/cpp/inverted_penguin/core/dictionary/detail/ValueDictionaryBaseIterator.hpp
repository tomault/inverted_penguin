#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBASEITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYBASEITERATOR_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <utility>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {
	template <typename Value> class ValueDictionaryImpl;

	template <typename Value, typename DerivedIterator>
	class ValueDictionaryBaseIterator {
	public:
	  typedef std::forward_iterator_tag iterator_category; 
	  typedef ptrdiff_t difference_type;

	protected:
	  typedef ValueDictionaryImpl<Value> DictionaryType;

	public:
	  DerivedIterator& operator++() {
	    ++index_;
	    return self_();
	  }

	  DerivedIterator operator++(int) {
	    DerivedIterator tmp(self_());
	    this->operator++();
	    return std::move(tmp);
	  }

	  bool operator==(const ValueDictionaryBaseIterator& other) const {
	    return (dict_ == other.dict_) && (index_ == other.index_);
	  }

	  bool operator!=(const ValueDictionaryBaseIterator& other) const {
	    return (dict_ != other.dict_) || (index_ != other.index_);
	  }

	protected:
	  const DictionaryType* dict_;
	  uint32_t index_;

	  ValueDictionaryBaseIterator(const DictionaryType* dict,
				      uint32_t index):
	      dict_(dict), index_(index) {
	  }

	  template <typename OtherIterator>
	  ValueDictionaryBaseIterator(
	      const ValueDictionaryBaseIterator<Value, OtherIterator>& other
	  ):
	      dict_(other.dict_), index_(other.index_) {
	  }

	  template <typename OtherIterator>
	  ValueDictionaryBaseIterator& operator=(
	      const ValueDictionaryBaseIterator<Value, OtherIterator>& other
	  ) {
	    if (this != &other) {
	      dict_ = other.dict_;
	      index_ = other.index_;
	    }
	    return *this;
	  }

	  const DerivedIterator& self_() const {
	    return static_cast<const DerivedIterator&>(*this);
	  }
	  
	  DerivedIterator& self_() {
	    return static_cast<DerivedIterator&>(*this);
	  }

	  ValueDictionaryKey getKey_() const { return dict_->keyAt(index_); }
	  const Value& getValue_() const { return dict_->valueAt(index_); }

	  template <typename V, typename O>
	  friend class ValueDictionaryBaseIterator;
	};
	
      }
    }
  }
}
#endif

