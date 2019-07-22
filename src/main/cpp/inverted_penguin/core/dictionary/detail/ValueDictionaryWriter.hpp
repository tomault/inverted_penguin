#ifndef __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYWRITER_HPP__
#define __INVERTED_PENGUIN__CORE__DICTIONARY__DETAIL__VALUEDICTIONARYWRITER_HPP__

#include <pistis/memory/AlignmentUtils.hpp>
#include <stdint.h>

namespace inverted_penguin {
  namespace core {
    namespace dictionary {
      namespace detail {

	template <typename Value>
	class ValueDictionaryWriter {
	public:
	  template <typename Iterator>
	  size_t dataSize(uint32_t numPairs, Iterator pairs) const {
	    using pistis::memory::alignmentPadding;
	    
	    size_t s = sizeof(uint32_t) + (numPairs * sizeof(uint32_t));
	    size_t totalKeyLength = 0;
	    auto p = pairs;
	    
	    for (uint32_t i = 0; i < numPairs; ++i, ++p) {
	      totalKeyLength += (*p).key.size();
	    }
	    return s + alignmentPadding(s, alignof(Value)) +
	           numPairs * sizeof(Value) + totalKeyLength;
	  }
	  
	  template <typename Iterator, typename DataWriter>
	  size_t write(uint32_t numPairs, Iterator pairs,
		       const DataWriter& writeData) const {
	    using pistis::memory::writePadding;
	    size_t numWritten = writeData(&numPairs, sizeof(uint32_t));
	    uint32_t keyEnd = (uint32_t)-1;
	    auto p = pairs;

	    for (uint32_t i = 0; i < numPairs; ++i, ++p) {
	      keyEnd += (*p).key.size();
	      numWritten += writeData(&keyEnd, sizeof(uint32_t));
	    }

	    numWritten += writePadding(numWritten, alignof(Value), writeData);
	    assert(!(numWritten & (alignof(Value) - 1)));

	    p = pairs;
	    for (uint32_t i = 0; i < numPairs; ++i, ++p) {
	      numWritten += writeData(&((*p).value), sizeof(Value));
	    }

	    p = pairs;
	    for (uint32_t i = 0; i < numPairs; ++i, ++p) {
	      const auto key = (*p).key;
	      numWritten += writeData(key.begin(), key.size());
	    }

	    return numWritten;
	  }
	};
	
      }
    }
  }
}
#endif

