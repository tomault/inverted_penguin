#include <inverted_penguin/terms/LowerCaseMapper.hpp>
#include <inverted_penguin/terms/IteratorRangeTermStream.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::terms;

namespace {

}

TEST(LowerCaseMapperTests, Apply) {
  LowerCaseMapper mapper;

  EXPECT_EQ(Term("abc", 5), mapper.apply(Term("ABC", 5)));
  EXPECT_EQ(Term("def", 2), mapper.apply(Term("def", 2)));
  EXPECT_EQ(Term("penguin", 3), mapper.apply(Term("Penguin", 3)));
}

TEST(LowerCaseMapperTests, Next) {
  std::vector<std::string> TERMS{
      "Mark", "was", "questioned", "by", "the", "FBI"
  };
  std::vector<Term> TRUTH{
    { "mark", 0 }, { "was", 1 }, { "questioned", 2 }, { "by", 3 },
    { "the", 4 }, { "fbi", 5 }
  };
  IteratorRangeTermStream< std::vector<std::string>::const_iterator > stream(
      TERMS.begin(), TERMS.end()
  );
  LowerCaseMapper mapper;
  std::vector<Term> result;
  Term t;

  while ((t = mapper.next(stream)).notEmpty()) {
    result.push_back(t);
  }

  EXPECT_EQ(TRUTH, result);  
}
