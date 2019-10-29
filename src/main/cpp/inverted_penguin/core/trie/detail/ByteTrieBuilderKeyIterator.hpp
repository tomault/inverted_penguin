#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderBaseIterator.hpp>
#include <string>
#include <vector>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderKeyIterator :
	public ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderKeyIterator<Value, BaseAllocator>
	> {
      private:
	typedef ByteTrieBuilderBaseIterator<
	    Value, BaseAllocator,
	    ByteTrieBuilderKeyIterator<Value, BaseAllocator>
	> Base;
					    
      public:
	using typename Base::Location;

	using typename Base::iterator_category;
	using typename Base::difference_type;
	typedef std::string value_type;
	typedef value_type reference;
	typedef void pointer;

      public:
	ByteTrieBuilderKeyIterator(): Base() { }
	ByteTrieBuilderKeyIterator(std::vector<Location>&& path):
	    Base(std::move(path_)) {
	}
	ByteTrieBuilderKeyIterator(const Base& other): Base(other) { }
	ByteTrieBuilderKeyIterator(Base&& other): Base(std::move(other)) { }

	reference operator*() const { return Base::makeKey_(this->path_); }

	ByteTrieBuilderKeyIterator& operator=(const Base& other) {
	  Base::operator=(other);
	  return *this;
	}
	
	ByteTrieBuilderKeyIterator& operator=(Base&& other) {
	  Base::operator=(std::move(other));
	  return *this;
	}
      };

    }
  }
}

#endif
