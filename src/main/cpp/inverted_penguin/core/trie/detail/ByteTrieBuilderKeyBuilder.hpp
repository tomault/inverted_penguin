#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYBUILDER_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYBUILDER_HPP__

#include <inverted_penguin/core/detail/ByteTrieBuilderKey.hpp>

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Allocator>
      class ByteTrieBuilderKeyBuilder {
      public:
	ByteTrieBuilderKeyBuilder(const Allocator& allocator = Allocator()):
	    ByteTrieBuilderKeyBuilder(16, allocator) {
	}
	ByteTrieBuilderKeyBuilder(size_t initialSize,
				  const Allocator& allocator = Allocator()):
	    Allocator(allocator), buffer_(allocator) {
	  buffer_.reserve(initialSize);
	}
	ByteTrieBuilderKeyBuilder(const ByteTrieBuilderKeyBuilder&) = default;
	ByteTrieBuilderKeyBuilder(ByteTrieBuilderKeyBuilder&&) = default;

	ByteTrieBuilderKeyBuilder& write(uint8_t v) {
	  buffer_.push_back(v);
	  return *this;
	}

	ByteTrieBuilderKeyBuilder& write(const uint8_t* begin,
					 const uint8_t* end) {
	  buffer_.insert(buffer_.end(), begin, end);
	  return *this;
	}
	
	ByteTrieBuilderKey<Allocator> done() {
	  return ByteTrieBuilderKey<Allocator>(buffer_.begin(), buffer_.end(),
					       buffer_.get_allocator());
	}

      private:
	std::vector<uint8_t, Allocator> buffer_;
      };
    }
  }
}
#endif

