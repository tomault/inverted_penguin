#include "SegmentBuilder.hpp"

using namespace inverted_penguin::segments;
using namespace inverted_penguin::terms;

// Index segment layout
// Offset        Size        Description
//      0           2        Byte order mark
//      2          10        Segment identifier
//     12           2        Index major version
//     14           2        Index minor version
//     16           8        Document lengths block start
//     24           8        Postings list start
//     32           8        Position list offset
//     40        4056        Unused - fills out header to page size
//   4096         CSL        Corpus statistics block
//    DLS         DLL        Document length block
//    POS         POL        Posting & position lists offsets
//    PLS         PLL        Posting & position lists
//
//
// Corpus statistics block
// Offset        Size        Description
//      0           4        Number of terms in segment  (=V)
//      4           4        Number of documents in segment (=N)
//      8           8        Sum of TF
//     16           4        Average document length (float)
//     20       4 * V        Doc frequency for each term
//
// Document length block
//      0       4 * N        Document lengths (sum of TF for each block)
//
// Postings & position list offsets block
//      0      16 * V        Paired offsets for postings and position lists,
//                           one for each term
//
// The postings and position lists are variable-length encoded deltas of
// document ids and positions.
//
// The segment size is:
//     4096           (header)
//  +  4 * V + 20     (corpus statistics)
//  +  4 * N          (document lengths)
//  + 16 * V          (offsets to posting & position lists)
//  + sum(postings list sizes)
//  + sum(position list sizes)
//
//  = 4116 + 20 * V + 4 * N + sum(posting list sizes) + sum(position list sizes)

/** @brief Size of an empty index 
 *
 *  The base index size is 4096 (the header block), plus 20 (base corpus
 *  statistics size), plus 20 (for term id 0) plus 4 (for document id 0).
 */
static const uint64_t INDEX_BASE_SIZE = 4140;
static const char INDEX_IDENTIFIER[] = "INVPEGINDX";
static const uint16_t MAJOR_VERSION = 0;
static const uint16_t MINOR_VERSION = 1;

SegmentBuilder(terms::TermDictionary& termDictionary):
    termDictionary_(termDictionary), postings_(), positions_(), docLengths_(),
    nextDocId_(1), numDocs_(0), sumOfTf_(0),
    indexSizeEstimate_(INDEX_BASE_SIZE) {
  postings_.reserve(16384);
  positions_.reserve(16384);
  docLengths_.reserve(16384);

  // Term id and document id 0 are reserved
  postings_.push_back(PostingsListBuilder());
  positions_.push_back(PositionListBuilder());
  docLengths_.push_back(0);
}

void SegmentBuilder::write(DataWriter& writer) {
  writer.seek(0);
  
}

PositionListBuilder& getPositionList_(uint32_t termId) {
  if (termId >= positions_.size()) {
    uint32_t delta = termId - postings_.size() + 1;
    postings_.resize(termId + 1);
    positions_.resize(termId + 1);
    indexSizeEstimate += 20 * delta;
  }
  return postings_[termId];
}


