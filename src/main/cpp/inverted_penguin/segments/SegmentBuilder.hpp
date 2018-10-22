#ifndef __INVERTED_PENGUIN__SEGMENTS__SEGMENTBUILDER_HPP__
#define __INVERTED_PENGUIN__SEGMENTS__SEGMENTBUILDER_HPP__

#include <inverted_penguin/core/DataWriter.hpp>
#include <inverted_penguin/segments/PostingsListBuilder.hpp>
#include <inverted_penguin/segments/PositionListBuilder.hpp>
#include <inverted_penguin/terms/TermDictionary.hpp>
#include <memory>

namespace inverted_penguin {
  namespace segments {

    class SegmentBuilder {
    public:
      struct DocStatistics {
	uint32_t docId;
	uint32_t numUniqueTerms_;
	uint64_t sumOfTf_;

	DocStatistics(uint32_t docId_, uint32_t numUniqueTerms_,
		      uint64_t sumOfTf_):
	    docId(docId_), numUniqueTerms(numUniqueTerms_), sumOfTf(sumOfTf_) {
	}
      };

    private:
      struct TermInfo {
	uint32_t id;
	uint64_t postingsIndexSize;
	uint64_t positionsIndexSize;

	TermInfo(uint32_t termId_, uint64_t postingsIndexSize_,
		 uint64_t positionsIndexSize_):
	    id(termId_), postingsIndexSize(postingsIndexSize_),
	    positionsIndexSize(positionsIndexSize_) {
	}
      };
	
    public:
      SegmentBuilder(terms::TermDictionary& termDictionary);
      SegmentBuilder(const SegmentBuilder&) = delete;
      SegmentBuilder(SegmentBuilder&&) = default;
      
      uint32_t numDocs() const { return numDocs_; }
      uint64_t sumOfTf() const { return sumOfTf_; }

      uint64_t indexSizeEstimate() const { return indexSizeEstimate_; }
      
      template <typename TermIterator>
      DocStatistics addDocument(TermIterator startOfTerms,
				TermIterator endOfTerms) {
	std::vector<TermInfo> termsInDoc;
	uint64_t sumOfTf = 0;

	for (auto i = startOfTerms; i != endOfTerms; ++i) {
	  auto term = *i;
	  auto& positions = getPositionList_(term.id());

	  if (!positions.docInProgress()) {
	    auto& postings = postings_[term.id()];
	    termsInDoc.push_back(TermInfo(term.id(),
					  postings.indexSizeEstimate(),
					  positions.indexSizeEstimate()));
	  }
	  positions.add(term.position());
	  ++sumOfTf;
	}

	for (const auto& term : termsInDoc) {
	  auto& postings = postings_[term.id];
	  auto& positions = positions_[term.id];

	  postings.add(nextDocId_, positions.numTermsInDoc());
	  positions.endDoc();

	  indexSizeEstimate_ +=
	      postings.indexSizeEstimate() - term.postingsIndexSize;
	  indexSizeEstimate_ +=
	      positions.indexSizeEstimate() - term.positionsIndexSize;
	}

	::assert(docLengths_.size() == nextDocId_);
	docLengths.append(sumOfTf);
	indexSizeEstimate_ += sizeof(uint32_t);
	++numDocs_;
	sumOfTf_ += sumOfTf;

	return DocStatistics(nextDocId++, termsInDoc.size(), sumOfTf);
      }

      void write(DataWriter& writer) const;

      SegmentBuilder& operator=(const SegmentBuilder&) = delete;
      SegmentBuilder& operator=(SegmentBuilder&&) = default;
      
    private:
      terms::TermDictionary& termDictionary_;
      std::vector<PostingsListBuilder> postings_;
      std::vector<PositionListBuilder> positions_;
      std::vector<uint32_t> docLengths_;
      uint32_t nextDocId_;
      uint32_t numDocs_;
      uint64_t sumOfTf_;
      uint64_t indexSizeEstimate_;

      PositionListBuilder& getPositionList_(uint32_t termId);
    };
  }
}
#endif
