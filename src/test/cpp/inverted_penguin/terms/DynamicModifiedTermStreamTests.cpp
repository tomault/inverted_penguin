#include <inverted_penguin/terms/DynamicModifiedTermStream.hpp>
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

namespace {
  typedef std::unique_ptr<DynamicTermStream> StreamPtr;
  typedef std::shared_ptr<DynamicTermStreamModifier> ModifierPtr;
}

TEST(DynamicModifiedTermStreamTests, CreateFromStreamAndModifier) {
  StreamPtr base(new DynamicVectorTermStream(
      std::vector<Term>({ Term("a-", 1), Term("b-", 2) }))
  );
  DynamicVectorTermStream* const termSource =
      (DynamicVectorTermStream*)base.get();
  ModifierPtr modifier(new DynamicSuffixAppender("MOO"));
  DynamicModifiedTermStream stream(std::move(base), modifier);

  EXPECT_EQ(termSource, stream.source());
  EXPECT_EQ(modifier, stream.modifier());
  EXPECT_EQ(Term("a-MOO", 1), stream.next());
  EXPECT_EQ(Term("b-MOO", 2), stream.next());
  EXPECT_EQ(Term::empty(), stream.next());
}

TEST(DynamicModifiedTermStreamTests, CreateByConcatenation) {
  StreamPtr base(new DynamicVectorTermStream(
      std::vector<Term>({ Term("a-", 1), Term("b-", 2) }))
  );
  DynamicVectorTermStream* const termSource =
      (DynamicVectorTermStream*)base.get();
  ModifierPtr modifier(new DynamicSuffixAppender("MOO"));
  StreamPtr stream(base + modifier);
  DynamicModifiedTermStream* const dynamicStream =
      (DynamicModifiedTermStream*)stream.get();

  ASSERT_TRUE(dynamicStream);
  EXPECT_NE(termSource, dynamicStream->source());
  EXPECT_EQ(modifier, dynamicStream->modifier());
  EXPECT_EQ(Term("a-MOO", 1), stream->next());
  EXPECT_EQ(Term("b-MOO", 2), stream->next());
  EXPECT_EQ(Term::empty(), stream->next());
}

TEST(DynamicModifiedTermStreamTests, CreateByConcatenationWithoutCopy) {
  StreamPtr base(new DynamicVectorTermStream(
      std::vector<Term>({ Term("a-", 1), Term("b-", 2) }))
  );
  DynamicVectorTermStream* const termSource =
      (DynamicVectorTermStream*)base.get();
  ModifierPtr modifier(new DynamicSuffixAppender("MOO"));
  StreamPtr stream(std::move(base) + modifier);
  DynamicModifiedTermStream* const dynamicStream =
      (DynamicModifiedTermStream*)stream.get();

  ASSERT_TRUE(dynamicStream);
  EXPECT_EQ(termSource, dynamicStream->source());
  EXPECT_EQ(modifier, dynamicStream->modifier());
  EXPECT_EQ(Term("a-MOO", 1), stream->next());
  EXPECT_EQ(Term("b-MOO", 2), stream->next());
  EXPECT_EQ(Term::empty(), stream->next());
}

TEST(DynamicModifiedTermStreamTests, Reset) {
  StreamPtr base(new DynamicVectorTermStream(
      std::vector<Term>({ Term("a-", 1), Term("b-", 2) }))
  );
  DynamicVectorTermStream* const termSource =
      (DynamicVectorTermStream*)base.get();
  ModifierPtr modifier(new DynamicSuffixAppender("MOO"));
  DynamicModifiedTermStream stream(std::move(base), modifier);

  EXPECT_EQ(Term("a-MOO", 1), stream.next());
  EXPECT_EQ(Term("b-MOO", 2), stream.next());
  EXPECT_EQ(Term::empty(), stream.next());

  EXPECT_TRUE(stream.reset());

  EXPECT_EQ(Term("a-MOO", 1), stream.next());
  EXPECT_EQ(Term("b-MOO", 2), stream.next());
  EXPECT_EQ(Term::empty(), stream.next());
}

TEST(DynamicModifiedTermStreamTests, Duplicate) {
  StreamPtr base(new DynamicVectorTermStream(
      std::vector<Term>({ Term("a-", 1), Term("b-", 2) }))
  );
  DynamicVectorTermStream* const termSource =
      (DynamicVectorTermStream*)base.get();
  ModifierPtr modifier(new DynamicSuffixAppender("MOO"));
  DynamicModifiedTermStream stream(std::move(base), modifier);
  StreamPtr copy(stream.duplicate());

  EXPECT_EQ(termSource, stream.source());
  EXPECT_NE(stream.source(),
	    ((DynamicModifiedTermStream*)copy.get())->source());
  EXPECT_EQ(stream.modifier(),
	    ((DynamicModifiedTermStream*)copy.get())->modifier());

  EXPECT_EQ(Term("a-MOO", 1), stream.next());
  EXPECT_EQ(Term("b-MOO", 2), stream.next());
  EXPECT_EQ(Term::empty(), stream.next());

  EXPECT_EQ(Term("a-MOO", 1), copy->next());
  EXPECT_EQ(Term("b-MOO", 2), copy->next());
  EXPECT_EQ(Term::empty(), copy->next());
}
