#ifndef __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERCONSTITERATOR_HPP__
#define __INVERTED_PENGUIN__CORE__DETAIL__BYTETRIEBUILDERCONSTITERATOR_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderBaseIterator.hpp>
#include <vector>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderIterator;

      template <typename Value, typename BaseAllocator>
      class ByteTrieBuilderConstIterator :
	  public ByteTrieBuilderBaseIterator<
	      Value, BaseAllocator,
	      ByteTrieBuilderConstIterator<Value, BaseAllocator>
	  > {
      private:
	typedef ByteTrieBuilderBaseIterator<
            Value, BaseAllocator,
	    ByteTrieBuilderConstIterator<Value, BaseAllocator>
	> Base;
	typedef ByteTrieBuilderIterator<Value, BaseAllocator> MutableIterator;
	using typename Base::KeyValueBase;


      public:
	using typename Base::Location;

	class KeyValueReference : public KeyValueBase {
	public:
	  KeyValueReference(const std::vector<Location>& path):
	      KeyValueBase(path) {
	  }
	  KeyValueReference(const KeyValueReference&) = default;

	  const Value& value() const {
	    const Location& loc = this->path_.back();
	    assert(loc.position.type() == Position::AT_VALUE);
	    return loc.node->valueAt(loc.position.index());
	  }

	  KeyValueReference& operator=(const KeyValueReference&) = delete;
	};
	
	using typename Base::iterator_category;
	using typename Base::difference_type;
	typedef KeyValueReference value_type;
	typedef value_type reference;
	typedef void pointer;

      public:
	ByteTrieBuilderConstIterator(): Base() { }
	ByteTrieBuilderConstIterator(std::vector<Location>&& path):
	    Base(std::move(path)) {
	}
	ByteTrieBuilderConstIterator(const ByteTrieBuilderConstIterator&) =
            default;
	ByteTrieBuilderConstIterator(ByteTrieBuilderConstIterator&&) =
            default;
	ByteTrieBuilderConstIterator(const MutableIterator& other):
	    Base(other.path_) {
	  // Intentionally left blank
	}
	ByteTrieBuilderConstIterator(const MutableIterator& other):
	    Base(std::move(other.path_)) {
	  // Intentionally left blank
	}

	reference operator*() const { return reference(this->path_); }

	ByteTrieBuilderConstIterator& operator=(
	    const ByteTrieBuilderConstIterator<Value>&
	) = default;

	ByteTrieBuilderConstIterator& operator=(
	    ByteTrieBuilderConstIterator&&
	) = default;

	ByteTrieBuilderConstIterator& operator=(const MutableIterator& other) {
	  if (this != &other) {
	    this->path_ = other.path_;
	  }
	  return *this;
	}
	
	ByteTrieBuilderConstIterator& operator=(MutableIterator&& other) {
	  if (this != &other) {
	    this->path_ = std::move(other.path_);
	  }
	  return *this;
	}
      };
      
    }
  }
}

#endif
