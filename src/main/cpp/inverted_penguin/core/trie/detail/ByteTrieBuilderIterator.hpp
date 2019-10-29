#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderBaseIterator.hpp>
#include <vector>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderValueIterator;
      
      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderIterator :
	  public ByteTrieBuilderBaseIterator<
              Value, BaseAllocator,
	      ByteTrieBuilderIterator<Value, BaseAllocator>
	  > {
      private:
	typedef ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderIterator<Value, BaseAllocator>
	> Base;
	using typename Base::KeyValueBase;

	typedef ByteTrieBuilderValueIterator<Value, BaseAllocator>
	        ValueIterator;
	
      public:
	using typename Base::Location;
	using typename Base::ValueReference;

	class KeyValueReference : public KeyValueBase {
	public:
	  KeyValueReference(const std::vector<Location>& path):
	      KeyValueBase(path) {
	  }
	  KeyValueReference(const KeyValueReference&) = default;

	  ValueReference& value() const {
	    return ValueReference(this->path_.back());
	  }

	  KeyValueReference& operator=(const KeyValueReference&) = delete;
	};
	
	using typename Base::iterator_category;
	using typename Base::difference_type;
	typedef KeyValueReference value_type;
	typedef value_type reference;
	typedef void pointer;

      public:
	ByteTrieBuilderIterator(): Base() { }
	ByteTrieBuilderIterator(std::vector<Location>&& path):
	    Base(std::move(path_)) {
	}
	ByteTrieBuilderIterator(const ByteTrieBuilderIterator&) = default;
	ByteTrieBuilderIterator(ByteTrieBuilderIterator&&) = default;
	ByteTrieBuilderIterator(const ValueIterator& other):
	    Base(other) {
	}
	ByteTrieBuilderIterator(ValueIterator&& other):
	    Base(std::move(other)) {
	}

	reference operator*() const { return reference(this->path_); }

	ByteTrieBuilderIterator& operator=(const ByteTrieBuilderIterator&) =
            default;
	ByteTrieBuilderIterator& operator=(ByteTrieBuilderIterator&&) =
            default;

	ByteTrieBuilderIterator& operator=(const ValueIterator& other) {
	  Base::operator=(other);
	  return *this;
	}
	
	ByteTrieBuilderIterator& operator=(ValueIterator&& other) {
	  Base::operator=(std::move(other));
	  return *this;
	}
      };
      
    }
  }
}
#endif

