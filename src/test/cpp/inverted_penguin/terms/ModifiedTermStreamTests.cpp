#include <inverted_penguin/tokens/TokenStream.hpp>
#include <inverted_penguin/tokens/TokenStreamModifier.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::tokens;

namespace {
  class SimpleTokenStream : public TokenStream<SimpleTokenStream> {
  public:
    explicit SimpleTokenStream(const std::vector<Token>& tokens)
        : tokens_(tokens), current_(tokens_.begin()) {
    }
    explicit SimpleTokenStream(const std::initializer_list<Token>& tokens)
        : tokens_(tokens), current_(tokens_.begin()) {
    }
    SimpleTokenStream(const SimpleTokenStream& other)
        : tokens_(other.tokens_),
	  current_(tokens_.begin() + (other.current_ - other.tokens_.begin())) {
    }

    Token next() {
      if (current_ == tokens_.end()) {
	return Token::empty();
      } else {
	return *current_++;
      }
    }

    bool reset() {
      current_ = tokens_.begin();
      return true;
    }

    SimpleTokenStream& operator=(const SimpleTokenStream& other) {
      if (this != &other) {
	tokens_ = other.tokens_;
	current_ = tokens_.begin() + (other.current_ - other.tokens_.begin());
      }
      return *this;
    }
  private:
    std::vector<Token> tokens_;
    std::vector<Token>::const_iterator current_;
  };

  class SuffixModifier : public TokenStreamModifier<SuffixModifier> {
  public:
    template <typename S>
    Token next(TokenStream<S>& stream) const {
      return this->apply(stream.self().next());
    }

    Token apply(const Token& t) const {
      return t.notEmpty() ? Token(t.text + ":foo", t.position) : t;
    }
  };

  class WrappingModifier : public TokenStreamModifier<WrappingModifier> {
  public:
    template <typename S>
    Token next(TokenStream<S>& stream) const {
      return this->apply(stream.self().next());
    }

    Token apply(const Token& t) const {
      if (t.isEmpty()) {
	return t;
      } else {
	return Token("(" + t.text + ")", t.position);
      }
    }
  };

  inline std::ostream& operator<<(std::ostream& out, const Token& t) {
    return out << "(\"" << t.text << ", " << t.position << ")";
  }

}

TEST(ModifiedTokenStreamTests, ModifyTokenStream) {
  const std::vector<Token> TOKENS{
      { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  const std::vector<Token> TRUTH{
      { "Penguins:foo", 0 }, { "are:foo", 1 }, { "cute:foo", 2 }
  };
  SimpleTokenStream ts(TOKENS);
  ModifiedTokenStream< SimpleTokenStream, SuffixModifier > modified =
      ts + SuffixModifier();
  std::vector<Token> tokens;
  Token t;

  while ((t = modified.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);
}

TEST(ModifiedTokenStreamTests, MoreThanOneModifier) {
  const std::vector<Token> TOKENS{
      { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  const std::vector<Token> TRUTH{
      { "(Penguins:foo)", 0 }, { "(are:foo)", 1 }, { "(cute:foo)", 2 }
  };
  SimpleTokenStream ts(TOKENS);
  TokenStreamModifierSequence< SuffixModifier, WrappingModifier> modifiers =
      SuffixModifier() + WrappingModifier();
  ModifiedTokenStream< ModifiedTokenStream< SimpleTokenStream, SuffixModifier >,
		       WrappingModifier > final =
      ts + modifiers;
  std::vector<Token> tokens;
  Token t;
  
  while ((t = final.next()).notEmpty()) {
    tokens.push_back(t);
  }
  EXPECT_EQ(TRUTH, tokens);
  
}
