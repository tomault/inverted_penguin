#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__MMAPVALUEDICTIONARYIMPL_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__MMAPVALUEDICTIONARYIMPL_HPP__

#include <inverted_penguin/core/dictionary/detail/ValueDictionaryImpl.hpp>
#include <pistis/memory/MMapPtr.hpp>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class MMapValueDictionaryImpl : public ValueDictionaryImpl<Value> {
	public:
	  MMapValueDictionaryImpl(const pistis::memory::MMapPtr<uint8_t>& data,
				  size_t offset = 0):
	      ValueDictionaryImpl<Value>(data.get() + offset), dataPtr_(data) {
	  }
	  MMapValueDictionaryImpl(MMapValueDictionaryImpl&&) = default;

	  MMapValueDictionaryImpl& operator=(MMapValueDictionaryImpl&&) =
	      default;

	private:
	  pistis::memory::MMapPtr<uint8_t> dataPtr_;
	};

      }
    }
  }
}
#endif

