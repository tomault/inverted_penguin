#ifndef __INVERTED_PENGUIN__CORPUSSTATISTICSBUILDER_HPP__
#define __INVERTED_PENGUIN__CORPUSSTATISTICSBUILDER_HPP__

#include <inverted_penguin/core/Writer.hpp>
#include <vector>
#include <stdint.h>

namespace inverted_penguin {
  namespace segments {

    class CorpusStatisticsBuilder {
    public:
      CorpusStatisticsBuilder():
	numTerms_(0), numDocs_(0), sumOfTf_(0), df_(32768, 0) {
      }

      uint32_t numTerms() const { return numTerms_; }
      uint32_t numDocs() const { return numDocs_; }
      uint64_t sumOfTf() const { return sumOfTf_; }
      uint32_t df(uint32_t tokenId) const {
	return (tokenId < df_.size()) ? df_[tokenId] : 0;
      }

      template <typename TermIterator>
      void addDoc(uint32_t docLength, TermIterator startOfTerms,
		  TermIterator endOfTerms) {
	for (auto i = startOfTerms; i != endOfTerms; ++i) {
	  auto termId = *i;
	  if (termId_ >= numTerms_) {
	    numTerms_ = termId_ + 1;
	    if (termId >= df_.size()) {
	      df_.resize(termId + 1, 0);
	    }
	  }
	  ++df_[termId];
	}
	sumOfTf_ += docLength;
	++numDocs_;
      }
      
      void write(core::Writer& writer) const;

    private:
      uint32_t numTerms_;
      uint32_t numDocs_;
      uint64_t sumOfTf_;
      std::vector<uint32_t> df_;
    };
    
  }
}
#endif
