#include <inverted_penguin/tokens/LowerCaseMapper.hpp>
#include <inverted_penguin/tokens/IteratorRangeTokenStream.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::tokens;

namespace {

}

TEST(LowerCaseMapperTests, Apply) {
  LowerCaseMapper mapper;

  EXPECT_EQ(Token("abc", 5), mapper.apply(Token("ABC", 5)));
  EXPECT_EQ(Token("def", 2), mapper.apply(Token("def", 2)));
  EXPECT_EQ(Token("penguin", 3), mapper.apply(Token("Penguin", 3)));
}

TEST(LowerCaseMapperTests, Next) {
  std::vector<std::string> TOKENS{
      "Mark", "was", "questioned", "by", "the", "FBI"
  };
  std::vector<Token> TRUTH{
    { "mark", 0 }, { "was", 1 }, { "questioned", 2 }, { "by", 3 },
    { "the", 4 }, { "fbi", 5 }
  };
  IteratorRangeTokenStream< std::vector<std::string>::const_iterator > stream(
      TOKENS.begin(), TOKENS.end()
  );
  LowerCaseMapper mapper;
  std::vector<Token> result;
  Token t;

  while ((t = mapper.next(stream)).notEmpty()) {
    result.push_back(t);
  }

  EXPECT_EQ(TRUTH, result);  
}
