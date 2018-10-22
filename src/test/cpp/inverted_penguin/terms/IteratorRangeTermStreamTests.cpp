#include <inverted_penguin/tokens/IteratorRangeTokenStream.hpp>
#include <gtest/gtest.h>

#include <ostream>
#include <string>
#include <vector>

using namespace inverted_penguin::tokens;

namespace inverted_penguin {
  namespace tokens {

    inline std::ostream& operator<<(std::ostream& out, const Token& t) {
      return out << "(\"" << t.text << ", " << t.position << ")";
    }

  }
}

TEST(IteratorRangeTokenStreamTests, IterateOverRange) {
  const std::vector<std::string> WORDS{ "Penguins", "are", "cute" };
  const std::vector<Token> TRUTH{
    { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  std::vector<Token> tokens;
  IteratorRangeTokenStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Token t;

  while ((t = stream.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);
}

TEST(IteratorRangeTokenStreamTests, IterateOverEmptyRange) {
  const std::vector<std::string> WORDS{ };
  const std::vector<Token> TRUTH{ };
  std::vector<Token> tokens;
  IteratorRangeTokenStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Token t;

  while ((t = stream.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);
}

TEST(IteratorRangeTokenStreamTests, Reset) {
  const std::vector<std::string> WORDS{ "Penguins", "are", "cute" };
  const std::vector<Token> TRUTH{
    { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  std::vector<Token> tokens;
  IteratorRangeTokenStream< std::vector<std::string>::const_iterator > stream(
      WORDS.begin(), WORDS.end()
  );
  Token t;

  while ((t = stream.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);

  EXPECT_TRUE(stream.next().isEmpty());

  tokens.clear();
  stream.reset();
  while ((t = stream.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);  
}
