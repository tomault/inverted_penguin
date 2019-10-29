#include <inverted_penguin/terms/DynamicTermStreamWrapper.hpp>
#include <inverted_penguin/testing_/terms/VectorTermStream.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::terms;

TEST(DynamicTermStreamWrapperTests, CreateFromTerms) {
  const Term t1("A", 1);
  const Term t2("B", 2);
  const Term t3("C", 3);
  DynamicVectorTermStream stream(std::vector<Term>{ t1, t2, t3 });

  // Verify the stream was constructed correctly by iterating through it
  EXPECT_EQ(t1, stream.next());
  EXPECT_EQ(t2, stream.next());
  EXPECT_EQ(t3, stream.next());
  EXPECT_EQ(Term::empty(), stream.next());  
}

TEST(DynamicTermStreamWrapperTests, CreateByCopyingStream) {
  const Term t1("A", 1);
  const Term t2("B", 2);
  const Term t3("C", 3);
  const VectorTermStream source({ t1, t2, t3 });
  DynamicVectorTermStream stream(source);

  // Verify source stream has not been altered
  ASSERT_EQ(source.size(), 3);
  ASSERT_EQ(t1, source.termAt(0));
  ASSERT_EQ(t2, source.termAt(1));
  ASSERT_EQ(t3, source.termAt(2));

  // Verify the stream was constructed correctly by iterating through it
  EXPECT_EQ(source.termAt(0), stream.next());
  EXPECT_EQ(source.termAt(1), stream.next());
  EXPECT_EQ(source.termAt(2), stream.next());
  EXPECT_EQ(Term::empty(), stream.next());
}

TEST(DynamicTermStreamWrapperTests, CreateByMovingStream) {
  const Term t1("A", 1);
  const Term t2("B", 2);
  const Term t3("C", 3);
  VectorTermStream source({ t1, t2, t3 });
  DynamicVectorTermStream stream(std::move(source));

  // Source was moved and left empty
  EXPECT_TRUE(source.empty());

  // Verify the stream was constructed correctly by iterating through it
  EXPECT_EQ(t1, stream.next());
  EXPECT_EQ(t2, stream.next());
  EXPECT_EQ(t3, stream.next());
  EXPECT_EQ(Term::empty(), stream.next());  
}

TEST(DynamicTermStreamWrapperTests, Reset) {
  const Term t1("A", 1);
  const Term t2("B", 2);
  const Term t3("C", 3);
  DynamicVectorTermStream stream(std::vector<Term>{ t1, t2, t3 });

  // Read the stream to its end...
  EXPECT_EQ(t1, stream.next());
  EXPECT_EQ(t2, stream.next());
  EXPECT_EQ(t3, stream.next());
  EXPECT_EQ(Term::empty(), stream.next());

  // Reset it and read it again...
  EXPECT_TRUE(stream.reset());

  EXPECT_EQ(t1, stream.next());
  EXPECT_EQ(t2, stream.next());
  EXPECT_EQ(t3, stream.next());
  EXPECT_EQ(Term::empty(), stream.next());
}

TEST(DynamicTermStreamWrapperTests, Duplicate) {
  const Term t1("A", 1);
  const Term t2("B", 2);
  const Term t3("C", 3);
  DynamicVectorTermStream stream(std::vector<Term>{ t1, t2, t3 });
  std::unique_ptr<DynamicVectorTermStream> copy(stream.duplicate());

  // Read the stream and its duplicate to their respective ends...
  EXPECT_EQ(t1, stream.next());
  EXPECT_EQ(t2, stream.next());
  EXPECT_EQ(t3, stream.next());
  EXPECT_EQ(Term::empty(), stream.next());

  EXPECT_EQ(t1, copy->next());
  EXPECT_EQ(t2, copy->next());
  EXPECT_EQ(t3, copy->next());
  EXPECT_EQ(Term::empty(), copy->next());  
}
