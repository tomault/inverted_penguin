#include <inverted_penguin/terms/DynamicTermStreamModifierSequence.hpp>
#include <inverted_penguin/terms/DynamicTermStreamModifier.hpp>
#include <inverted_penguin/terms/NumberFilter.hpp>
#include <inverted_penguin/terms/TermStreamModifier.hpp>
#include <inverted_penguin/testing_/terms/PrefixPrepender.hpp>
#include <inverted_penguin/testing_/terms/SuffixAppender.hpp>
#include <inverted_penguin/testing_/terms/VectorTermStream.hpp>
#include <pistis/memory/PtrUtils.hpp>
#include <gtest/gtest.h>
#include <memory>

using namespace inverted_penguin::terms;
namespace pm = pistis::memory;

namespace {
  typedef std::shared_ptr<DynamicTermStreamModifier> ModifierPtr;
}

// TODO: Verify that isStateful() and duplicate() behave correctly with
//           stateful modifiers
//       Verify that reset() behaves correctly with modifiers that cannot
//           be reset

TEST(DynamicTermStreamModifierSequenceTests, ConstructFromModifiers) {
  ModifierPtr first(new DynamicPrefixPrepender("MOO"));
  ModifierPtr second(new DynamicSuffixAppender("PEGU"));
  DynamicTermStreamModifierSequence seq(first, second);
  const Term t("-x-", 15);
  const Term truth("MOO-x-PEGU", 15);

  EXPECT_EQ(first, seq.firstModifier());
  EXPECT_EQ(second, seq.secondModifier());
  EXPECT_FALSE(seq.isStateful());
  EXPECT_EQ(truth, seq.apply(t));
  EXPECT_EQ(Term::empty(), seq.apply(Term::empty()));
}

TEST(DynamicTermStreamModifierSequenceTests, ConstructByConcatenation) {
  ModifierPtr first(new DynamicPrefixPrepender("MOO"));
  ModifierPtr second(new DynamicSuffixAppender("PEGU"));
  ModifierPtr seq = first + second;
  DynamicTermStreamModifierSequence* const seqModifier =
      (DynamicTermStreamModifierSequence*)seq.get();
  const Term t("-x-", 15);
  const Term truth("MOO-x-PEGU", 15);

  EXPECT_EQ(first, seqModifier->firstModifier());
  EXPECT_EQ(second, seqModifier->secondModifier());
  EXPECT_FALSE(seq->isStateful());
  EXPECT_EQ(truth, seq->apply(t));
  EXPECT_EQ(Term::empty(), seq->apply(Term::empty()));
}

TEST(DynamicTermStreamModifierSequenceTests, Next) {
  ModifierPtr first(new DynamicPrefixPrepender("MOO"));
  ModifierPtr second(new DynamicSuffixAppender("PEGU"));
  DynamicTermStreamModifierSequence seq(first, second);
  DynamicVectorTermStream stream(
     std::vector<Term>{ { "-a-", 1 }, { "-b-", 2 } }
  );

  EXPECT_EQ(Term("MOO-a-PEGU", 1), seq.next(stream));
  EXPECT_EQ(Term("MOO-b-PEGU", 2), seq.next(stream));
  EXPECT_EQ(Term::empty(), seq.next(stream));
}

TEST(DynamicTermStreamModifierSequenceTests, NextWithFilter) {
  ModifierPtr first(new DynamicNumberFilter());
  ModifierPtr second(new DynamicSuffixAppender("MOO"));
  DynamicTermStreamModifierSequence seq(first, second);
  DynamicVectorTermStream stream(
      std::vector<Term>{ { "a-", 1 }, {"100", 2}, { "b-", 3 } }
  );

  EXPECT_EQ(Term("a-MOO", 1), seq.next(stream));
  EXPECT_EQ(Term("b-MOO", 3), seq.next(stream));
  EXPECT_EQ(Term::empty(), seq.next(stream));
}

TEST(DynamicTermStreamModifierSequenceTests, Reset) {
  ModifierPtr first(new DynamicPrefixPrepender("MOO"));
  ModifierPtr second(new DynamicSuffixAppender("PEGU"));
  DynamicTermStreamModifierSequence seq(first, second);

  EXPECT_TRUE(seq.reset());
}

TEST(DynamicTermStreamModifierSequenceTests, Duplicate) {
  ModifierPtr first(new DynamicPrefixPrepender("MOO"));
  ModifierPtr second(new DynamicSuffixAppender("PEGU"));
  DynamicTermStreamModifierSequence seq(first, second);
  std::unique_ptr<DynamicTermStreamModifierSequence> copy(seq.duplicate());
  
  
}
