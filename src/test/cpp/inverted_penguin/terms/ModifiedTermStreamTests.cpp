#include <inverted_penguin/terms/TermStream.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::terms;

namespace {
  class SimpleTermStream : public TermStream<SimpleTermStream> {
  public:
    explicit SimpleTermStream(const std::vector<Term>& terms)
        : terms_(terms), current_(terms_.begin()) {
    }
    explicit SimpleTermStream(const std::initializer_list<Term>& terms)
        : terms_(terms), current_(terms_.begin()) {
    }
    SimpleTermStream(const SimpleTermStream& other)
        : terms_(other.terms_),
	  current_(terms_.begin() + (other.current_ - other.terms_.begin())) {
    }

    Term next() {
      if (current_ == terms_.end()) {
	return Term::empty();
      } else {
	return *current_++;
      }
    }

    bool reset() {
      current_ = terms_.begin();
      return true;
    }

    SimpleTermStream& operator=(const SimpleTermStream& other) {
      if (this != &other) {
	terms_ = other.terms_;
	current_ = terms_.begin() + (other.current_ - other.terms_.begin());
      }
      return *this;
    }
  private:
    std::vector<Term> terms_;
    std::vector<Term>::const_iterator current_;
  };

  class SuffixModifier : public TermStreamModifier<SuffixModifier> {
  public:
    template <typename S>
    Term next(TermStream<S>& stream) const {
      return this->apply(stream.self().next());
    }

    Term apply(const Term& t) const {
      return t.notEmpty() ? Term(t.text + ":foo", t.position) : t;
    }
  };

  class WrappingModifier : public TermStreamModifier<WrappingModifier> {
  public:
    template <typename S>
    Term next(TermStream<S>& stream) const {
      return this->apply(stream.self().next());
    }

    Term apply(const Term& t) const {
      if (t.isEmpty()) {
	return t;
      } else {
	return Term("(" + t.text + ")", t.position);
      }
    }
  };

}

TEST(ModifiedTermStreamTests, ModifyTermStream) {
  const std::vector<Term> TERMS{
      { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  const std::vector<Term> TRUTH{
      { "Penguins:foo", 0 }, { "are:foo", 1 }, { "cute:foo", 2 }
  };
  SimpleTermStream ts(TERMS);
  ModifiedTermStream< SimpleTermStream, SuffixModifier > modified =
      ts + SuffixModifier();
  std::vector<Term> terms;
  Term t;

  while ((t = modified.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);
}

TEST(ModifiedTermStreamTests, MoreThanOneModifier) {
  const std::vector<Term> TERMS{
      { "Penguins", 0 }, { "are", 1 }, { "cute", 2 }
  };
  const std::vector<Term> TRUTH{
      { "(Penguins:foo)", 0 }, { "(are:foo)", 1 }, { "(cute:foo)", 2 }
  };
  SimpleTermStream ts(TERMS);
  TermStreamModifierSequence< SuffixModifier, WrappingModifier> modifiers =
      SuffixModifier() + WrappingModifier();
  ModifiedTermStream< ModifiedTermStream< SimpleTermStream, SuffixModifier >,
		       WrappingModifier > final =
      ts + modifiers;
  std::vector<Term> terms;
  Term t;
  
  while ((t = final.next()).notEmpty()) {
    terms.push_back(t);
  }
  EXPECT_EQ(TRUTH, terms);
  
}
