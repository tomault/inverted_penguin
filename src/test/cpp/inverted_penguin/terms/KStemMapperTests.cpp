#include <inverted_penguin/terms/KStemMapper.hpp>
#include <gtest/gtest.h>

#include <inverted_penguin/terms/IteratorRangeTermStream.hpp>

#include <iostream>

using namespace inverted_penguin::terms;

TEST(KStemMapper, Apply) {
  const std::string LONG_WORD(
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  );
  KStemMapper mapper;

  EXPECT_EQ(Term("a", 5), mapper.apply(Term("a", 5)));
  EXPECT_EQ(Term(LONG_WORD, 2), mapper.apply(Term(LONG_WORD, 2)));
  EXPECT_EQ(Term("penguin", 3), mapper.apply(Term("penguin", 3)));
  EXPECT_EQ(Term("walk", 4), mapper.apply(Term("walks", 4)));
}

TEST(KStemMapper, Next) {
  std::vector<std::string> TERMS{
      "mark", "was", "questioned", "by", "the", "fbi"
  };
  std::vector<Term> TRUTH{
    { "mark", 0 }, { "was", 1 }, { "question", 2 }, { "by", 3 },
    { "the", 4 }, { "fbi", 5 }
  };
  IteratorRangeTermStream< std::vector<std::string>::const_iterator > stream(
      TERMS.begin(), TERMS.end()
  );
  KStemMapper mapper;
  std::vector<Term> result;
  Term t;

  while ((t = mapper.next(stream)).notEmpty()) {
    result.push_back(t);
  }

  EXPECT_EQ(TRUTH, result);  


}

