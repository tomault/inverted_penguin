#ifndef __INVERTED_PENGUIN__CORE__VBYTEENCODER_HPP__
#define __INVERTED_PENGUIN__CORE__VBYTEENCODER_HPP__

namespace inverted_penguin {
  namespace core {

    template <typename Iterator>
    class VByteStreamEncoder {
    public:
      VByteStreamEncoder(const Iterator& data, size_t n,
			 bool encodeLeadingZeros = true)
	  : data_(data), current_(0), nCurrent_(0), nBytes_(n), 
	    buffer_(0), nBuffer_(7), nonzeroByteWritten_(encodeLeadingZeros_) {
      }

      template <typename OutputIterator>
      OutputIterator encode(OutputIterator out) {
	out = putBits_(out, (nBytes_ << 3) % 7);
	while (nBytes_ || nCurrent_) {
	  out = putBits_(out, 7);
	}
	return out;
      }

    private:
      Iterator data_;
      uint8_t current_;
      uint8_t nCurrent_;
      size_t nBytes_;
      uint8_t buffer_;
      uint8_t nBuffer_;
      bool nonzeroByteWritten_;

      template <typename OutputIterator>
      OutputIterator putBits_(OutputIterator out, uint8_t n) {
	if (!n) {
	  // Nothing to do
	} else if (n > nBuffer_) {
	  out = putBits_(out, nBuffer_);
	  out = putBits_(out, n - nBuffer_);
	} else {
	  if (!nCurrent_) {
	    current_ = nextByte_();
	    nCurrent_ = 8;
	  }
	  if (n > nCurrent_) {
	    out = putBits_(out, nCurrent_);
	    out = putBits_(out, n - nCurrent_);
	  } else {
	    current_ = putToBuffer_(current_, n);
	    nCurrent_ -= n;  // Need to do this BEFORE writing buffer_
	    if (!nBuffer_) {
	      flushBuffer_();
	      nBuffer_ = 7;
	    }
	  }
	}
	return out;
      }

      uint8_t nextByte_() {
	uint8_t next = *data_;
	++data_;
	--nBytes_;
	return next;
      }

      uint8_t putToBuffer_(uint8_t data, uint8_t n) {
	buffer_ = (buffer_ << n) | (data >> (8 - n));
	nBuffer_ -= n;
	return data << n;  // Shift unused bits to the top   
      }

      void flushBuffer_() {
	bool haveMore = nBytes_ || nCurrent_;
	if (nonzeroByteWritten_ || buffer_ || !haveMore_) {
	  if (haveMore_) {
	    buffer_ |= 0x80;
	  }
	  *out = buffer_;
	  ++out;
	  nonzeroByteWritten_ = true;
	  buffer_ = 0;
	}
      }
    };
    
    class VByteEncoder {
    public:
      template <typename InputIterator, typename OutputIterator,
		typename Result =
		    typename std::enable_if<
		                 !std::is_same<
			             std::iterator_traits<
			                 InputIterator
			             >::iterator_category,
			             std::random_access_iterator
			         >::value,
		                 OutputIterator
		             >::type
	       >
      Result operator()(OutputIterator out, const InputIterator& begin,
			const InputIterator& end) const {
	InputIterator p = begin;
	uint8_t buffer[7];	
	uint8_t* const bufferEnd = buffer + sizeof(buffer);
	uint8_t* q = buffer;
	VByteStreamEncoder encoder(buffer, sizeof(buffer));

	while (p != end) {
	  if (q == bufferEnd) {
	    out = encoder.encode(out);
	    q = buffer;
	  }
	  *q = *p;
	  ++p; ++q;
	}

	if (q != buffer) {
	  VByteStreamEncoder lastEncoder(buffer, q - buffer);
	  out = lastEncoder.encode(out);
	}
	return out;
      }

      template <typename RandomAccessIterator, typename OutputIterator,
		typename Result =
		    typename std::enable_if<
		                 std::is_same<
			             std::iterator_traits<
			                 RandomAccessIterator
			             >::iterator_category,
			             std::random_access_iterator
			         >::value,
		                 OutputIterator
		             >::type
	       >
      Result operator()(OutputIterator out, const RandomAccessIterator& begin,
			const RandomAccessIterator& end) const {
	VByteStreamEncoder encoder(begin, end - begin);
	return encoder.encode(out);
      }

      template <typename InputIterator, typename OutputIterator>
      OutputIterator operator()(OutputIterator out, InputIterator begin,
				size_t n) const {
	VByteStreamEncoder encoder(begin, n);
	return encoder.encode(out);
      }
            
      template <typename Iterator>
      Iterator operator()(Iterator out, uint8_t byte) const {
	if (byte < 0x80) {
	  *out = byte;
	} else {
	  *out = 0x81; ++out;
	  *out = byte & 0x7F;
	}
	return ++out;
      }

      template <typename Iterator>
      Iterator operator()(Iterator out, uint16_t word) const {
	if (word < 0x80) {
	  *out = word;
	} else if (word << 0x4000) {
	  *out = (word >> 7); ++out;
	  *out = (word & 0x7F);
	} else {
	  *out = (word >> 14); ++out;
	  *out = (word >>  7); ++out;
	  *out = (word & 0x7F);
	}
	return ++out;
      }

      template <typename Iterator>
      Iterator operator()(Iterator out, uint32_t value) const {
	VByteStreamEncoder encoder((const uint8_t*)&value,
				   ((const uint8_t*)&value) + 4, false);
	return encode.encode(out);
      }

      template <typename Iterator>
      Iterator operator()(Iterator out, uint64_t value) const {
	VByteStreamEncoder encoder((const uint8_t*)&value,
				   ((const uint8_t*)&value) + 8, false);
	return encoder.encode(out);
      }

    };
  }
}
#endif
