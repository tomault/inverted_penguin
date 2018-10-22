#include <inverted_penguin/tokens/KStemMapper.hpp>
#include <gtest/gtest.h>

#include <inverted_penguin/tokens/IteratorRangeTokenStream.hpp>

#include <iostream>

using namespace inverted_penguin::tokens;

namespace inverted_penguin {
  namespace tokens {

    inline std::ostream& operator<<(std::ostream& out, const Token& t) {
      return out << "(\"" << t.text << ", " << t.position << ")";
    }

  }
}

TEST(KStemMapper, Apply) {
  const std::string LONG_WORD(
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  );
  KStemMapper mapper;

  EXPECT_EQ(Token("a", 5), mapper.apply(Token("a", 5)));
  EXPECT_EQ(Token(LONG_WORD, 2), mapper.apply(Token(LONG_WORD, 2)));
  EXPECT_EQ(Token("penguin", 3), mapper.apply(Token("penguin", 3)));
  EXPECT_EQ(Token("walk", 4), mapper.apply(Token("walks", 4)));
}

TEST(KStemMapper, Next) {
  std::vector<std::string> TOKENS{
      "mark", "was", "questioned", "by", "the", "fbi"
  };
  std::vector<Token> TRUTH{
    { "mark", 0 }, { "was", 1 }, { "question", 2 }, { "by", 3 },
    { "the", 4 }, { "fbi", 5 }
  };
  IteratorRangeTokenStream< std::vector<std::string>::const_iterator > stream(
      TOKENS.begin(), TOKENS.end()
  );
  KStemMapper mapper;
  std::vector<Token> result;
  Token t;

  while ((t = mapper.next(stream)).notEmpty()) {
    result.push_back(t);
  }

  EXPECT_EQ(TRUTH, result);  


}

