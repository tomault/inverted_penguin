#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__VALUEDICTIONARY_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__VALUEDICTIONARY_HPP__

#include <inverted_penguin/core/Optional.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryImpl.hpp>
#include <inverted_penguin/core/dictionary/detail/InMemoryValueDictionaryImpl.hpp>
#include <inverted_penguin/core/dictionary/detail/MMapValueDictionaryImpl.hpp>
#include <tuple>
#include <unordered_map>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {

      template <typename Value, typename Impl>
      class ValueDictionary {
      public:
	typedef typename Impl::Key Key;
	typedef typename Impl::Iterator ConstIterator;
	typedef ConstIterator Iterator;
	typedef typename Impl::KeyIterator KeyIterator;
	typedef typename Impl::ValueIterator ConstValueIterator;
	typedef ConstValueIterator ValueIterator;

      public:
	template <typename... Args>
	ValueDictionary(Args... args):
	    impl_(std::forward<Args>(args)...), cache_() {
	  for (uint32_t i = 0; i < impl_.size(); ++i) {
	    cache_.emplace(impl_.keyAt(i), i);
	  }
	}
	ValueDictionary(const ValueDictionary&) = delete;
	ValueDictionary(ValueDictionary&&) = default;
	
	uint32_t size() const { return impl_.size(); }
	ConstIterator begin() const { return impl_.begin(); }
	ConstIterator end() const { return impl_.end(); }
	ConstIterator cbegin() const { return begin(); }
	ConstIterator cend() const { return end(); }
	ConstIterator find(const uint8_t* key, const uint8_t* keyEnd) const {
	  auto p = cache_.find(Key(key, keyEnd));
	  return (p != cache_.end()) ? impl_.position(p->second) : end();
	}

	std::tuple<ConstIterator, ConstIterator> prefix(
	    const uint8_t* key, const uint8_t* keyEnd
	) const {
	  return impl_.prefix(key, keyEnd);
	}
	
	Optional<Value> get(const uint8_t* key, const uint8_t* keyEnd) const {
	  auto p = cache_.find(Key(key, keyEnd));
	  return (p != cache_.end()) ?
	      Optional<Value>(impl_.valueAt(p->second)) :
	      Optional<Value>();
	}

	ValueDictionary& operator=(const ValueDictionary&) = delete;
	ValueDictionary& operator=(ValueDictionary&&) = default;

      protected:
	Impl impl_;
	std::unordered_map<Key, uint32_t> cache_;
      };

      template <typename Value, typename Allocator>
      using InMemoryValueDictionary =
	  ValueDictionary<
	      Value,
	      detail::InMemoryValueDictionaryImpl<Value, Allocator>
	  >;

      template <typename Value>
      using MMapValueDictionary =
	  ValueDictionary< Value, detail::MMapValueDictionaryImpl<Value> >;

      template <typename Value>
      using UnmanagedValueDictionary =
	  ValueDictionary< Value, detail::ValueDictionaryImpl<Value> >;
      
    }
  }
}

#endif
