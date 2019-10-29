#ifndef __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYVALUE_HPP__
#define __INVERTEDPENGUIN__CORE__DETAIL__BYTETRIEBUILDERKEYVALUE_HPP__

namespace inverted_penguin {
  namespace core {
    namespace detail {

      template <typename Value, typename Allocator>
      class ByteTrieBuilderKeyValue {
      public:
	ByteTrieBuilderKeyValue(const Allocator& allocator = Allocator()):
	    key_(allocator), value_() {
	}
	
	ByteTrieBuilderKeyValue(const ByteTrieBuilderKey<Allocator>& key,
				const Value& value):
	    key_(key), value_(value) {
	}
	
	ByteTrieBuilderKeyValue(ByteTrieBuilderKey<Allocator>&& key,
				const Value& value):
	    key_(std::move(key)), value_(value) {
	}	ByteTrieBuilderKeyValue(ByteTrieBuilderKey<Allocator>&& key,
				Value&& value):
	    key_(std::move(key)), value_(std::move(value)) {
	}
	
	ByteTrieBuilderKeyValue(const ByteTrieBuilderKeyValue& other)
	    = default;

	ByteTrieBuilderKeyValue(ByteTrieBuilderKeyValue&& other) = default;

	const ByteTrieBuilderKey<Allocator>& key() const { return key_; }
	const Value& value() const { return value_; }

	void setValue(const Value& v) { value_ = v; }
	void setValue(Value&& v) { value_ = std::move(v); }

	const ByteTrieBuilderKeyValue& operator=(
	    const ByteTrieBuilderKeyValue&
	) = default;

	const ByteTrieBuilderKeyValue& operator=(ByteTrieBuilderKeyValue&&)
	    = default;
	
      private:
	ByteTrieBuilderKey<Allocator> key_;
	Value value_;
      };
      
    }
  }
}
#endif

