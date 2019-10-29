#ifndef __INVERTED_PENGUIN__IO__FILEINPUTBUFFER_HPP__
#define __INVERTED_PENGUIN__IO__FILEINPUTBUFFER_HPP__

#include <pistis/filesystem/File.hpp>
#include <memory>
#include <string.h>

namespace inverted_penguin {
  namespace io {

    class FileInputBuffer {
    public:
      static constexpr const size_t DEFAULT_BUFFER_SIZE = 65536;
      
    public:
      FileInputBuffer(pistis::filesystem::File&& file,
		      size_t defaultBufferSize = DEFAULT_BUFFER_SIZE):
	  file_(std::move(file)), buffer_(), end_(buffer_.get()),
	  bufferSize_(defaultBufferSize) {
      }

      FileInputBuffer(const std::string& name,
		      size_t defaultBufferSize = DEFAULT_BUFFER_SIZE):
	  FileInputBuffer(
	      pistis::filesystem::File::open(
	          name, pistis::filesystem::FileAccessMode::READ_ONLY
	      ), defaultBufferSize
	  ) {
      }

      FileInputBuffer(const FileInputBuffer&) = delete;
      FileInputBuffer(FileInputBuffer&&) = default;

      size_t size() const { return bufferSize_; }
      size_t occupied() const { return end_ - buffer_.get(); }

      const char* begin() {
	fill_();
	return buffer_.get();
      }

      const char* end() {
	fill_();
	return end_;
      }

      size_t fill(size_t numToSave = 0) {
	if (numToSave) {
	  ::memmove(buffer_.get(), end_ - numToSave, numToSave);
	}
	end_ = buffer_.get() + numToSave;
	return fill_();
      }

      size_t resize(size_t requested) {
	if (requested > (end_ - buffer_.get())) {
	  std::unique_ptr<char[]> newBuffer_(new char[requested]);
	  if (end_ > buffer_.get()) {
	    ::memcpy(newBuffer_.get(), buffer_.get(), end_ - buffer_.get());
	  }
	  end_ = newBuffer_.get() + (end_ - buffer_.get());
	  buffer_ = std::move(newBuffer);
	  bufferSize_ = requested;
	  fill_();
	}
	return bufferSize_;
      }

      FileInputBuffer& operator=(const FileInputBuffer&) = delete;
      FileInputBuffer& operator=(FileInputBuffer&&) = default;
      
    private:
      pistis::filesystem::File file_;
      std::unique_ptr<char[]> buffer_;
      char* end_;
      size_t bufferSize_;

      size_t fill_() {
	if (!buffer_) {
	  buffer_.reset(new char[bufferSize_]);
	  end_ = buffer_.get();
	}

	char* const bufferEnd_ = buffer_.get() + bufferSize_;
	assert(end_ <= bufferEnd_);
	if (end_ < bufferEnd_) {
	  size_t n = file_.read(end_, bufferEnd_ - end);
	  end_ += n;
	}
	return occupied();
      }
    };
    
  }
}
#endif

