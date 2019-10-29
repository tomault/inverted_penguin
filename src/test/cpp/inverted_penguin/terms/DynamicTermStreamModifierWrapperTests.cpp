#include <inverted_penguin/terms/DynamicTermStreamModifierWrapper.hpp>
#include <inverted_penguin/testing_/terms/SuffixAppender.hpp>
#include <inverted_penguin/testing_/terms/VectorTermStream.hpp>
#include <gtest/gtest.h>
#include <vector>

using namespace inverted_penguin::terms;

TEST(DynamicTermStreamModifierWrapperTests, CreateFromArgs) {
  DynamicSuffixAppender appender("MOO");

  ASSERT_FALSE(appender.isStateful());
  EXPECT_EQ(Term("daijouMOO", 10), appender.apply(Term("daijou", 10)));
  EXPECT_EQ(Term::empty(), appender.apply(Term::empty()));
}

TEST(DynamicTermStreamModifierWrapperTests, CreateByCopyingModifier) {
  SuffixAppender source("MOO");
  DynamicSuffixAppender appender(source);

  EXPECT_EQ(Term("daijouMOO", 10), appender.apply(Term("daijou", 10)));
}

TEST(DynamicTermStreamModifierWrapperTests, CreateByMovingModifier) {
  SuffixAppender source("MOO");
  DynamicSuffixAppender appender(std::move(source));

  EXPECT_EQ(Term("daijouMOO", 10), appender.apply(Term("daijou", 10)));
}

TEST(DynamicTermStreamModifierWrapperTests, Next) {
  DynamicSuffixAppender appender("MOO");
  DynamicVectorTermStream stream(std::vector<Term>{ Term("a", 1),
	                                            Term("b", 2) });

  EXPECT_EQ(Term("aMOO", 1), appender.next(stream));
  EXPECT_EQ(Term("bMOO", 2), appender.next(stream));
  EXPECT_EQ(Term::empty(), appender.next(stream));
}

TEST(DynamicTermStreamModifierWrapperTests, Reset) {
  DynamicSuffixAppender appender("MOO");
  EXPECT_TRUE(appender.reset());
}

TEST(DynamicTermStreamModifierWrapperTests, Duplicate) {
  DynamicSuffixAppender appender("MOO");
  std::unique_ptr<DynamicSuffixAppender> copy(
      (DynamicSuffixAppender*)appender.duplicate()
  );
  const Term t("daijou", 10);
  const Term truth("daijouMOO", 10);

  EXPECT_EQ(truth, appender.apply(t));
  EXPECT_EQ(truth, copy->apply(t));
}
