#ifndef __INVERTED_PENGUIN__IO__MEMORYINPUTBUFFER_HPP__
#define __INVERTED_PENGUIN__IO__MEMORYINPUTBUFFER_HPP__

#include <stddef.h>

namespace inverted_penguin {
  namespace io {

    class MemoryInputBuffer {
    public:
      MemoryInputBuffer(const char* begin, const char* end):
	  begin_(begin), end_(end) {
      }

      size_t size() const { return end_ - begin_; }
      const char* begin() const { return begin_; }
      const char* end() const { return end_; }

      size_t fill(size_t numToSave = 0) {
	if (numToSave) {
	  ::memmove(begin_, end_ - numToSave, numToSave);
	}
	end_ = begin_ + numToSave;
	return 0;
      }

      size_t resize(size_t requested) { return size(); }

    private:
      const char* begin_;
      const char* end_;
    };
    
  }
}
#endif

