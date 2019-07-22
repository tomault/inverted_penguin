#include <inverted_penguin/core/dictionary/detail/ValueDictionaryPersistentKey.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <sstream>

using namespace inverted_penguin::core::dictionary::detail;
namespace pt = pistis::testing;

namespace {
  typedef pt::Allocator<uint8_t> TestAllocator;
  typedef ValueDictionaryPersistentKey<TestAllocator> TestKey;

  /** @brief Use if key and truth are known to be the same length */
  template <typename Allocator>
  ::testing::AssertionResult verifyContent(
      const ValueDictionaryPersistentKey<Allocator>& key, const uint8_t* truth
  ) {
    for (uint32_t i = 0; i < key.size(); ++i) {
      if (key[i] != truth[i]) {
	return ::testing::AssertionFailure()
	    << "Keys are not equal; key[" << i << "] is 0x"
	    << std::hex << std::setw(2) << std::setfill('0') << key[i]
	    << ", but it should be 0x" << std::hex << std::setw(2)
	    << std::setfill('0') << truth[i];
      }
    }
    return ::testing::AssertionSuccess();
  }

  /** @brief Use if key and truth might have different lengths */
  template <typename Allocator>
  ::testing::AssertionResult verifyContent(
     const ValueDictionaryPersistentKey<Allocator>& key, const uint8_t* truth,
     const uint8_t* truthEnd
  ) {
    if (key.size() != (truthEnd - truth)) {
      return ::testing::AssertionFailure()
	  << "Key has length " << key.size() << ", but it should have length "
	  << (truthEnd - truth);
    }
    return verifyContent(key, truth);
  }
}

TEST(ValueDictionaryPersistentKeyTests, ConstructDefault) {
  TestKey key;
  EXPECT_EQ(0, key.size());
  EXPECT_EQ(key.end(), key.begin());
}

TEST(ValueDictionaryPersistentKeyTests, ConstructFromRange) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  TestAllocator allocator("TEST");
  TestKey key(BYTES, BYTES + sizeof(BYTES), allocator);

  EXPECT_EQ("TEST", key.allocator().name());
  EXPECT_FALSE(key.allocator().moved());

  ASSERT_EQ(sizeof(BYTES), key.size());
  EXPECT_EQ(sizeof(BYTES), (key.end() - key.begin()));
  EXPECT_NE(BYTES, key.begin());
  EXPECT_NE(BYTES + sizeof(BYTES), key.end());

  EXPECT_TRUE(verifyContent(key, BYTES));
}

// TEST(ValueDictionaryPersistentKeyTests, ConstructFromCopy) {
//   const uint8_t BYTES[] = {
//       'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
//   };
//   TestAllocator allocator("TEST");
//   TestKey key(BYTES, BYTES + sizeof(BYTES), allocator);

//   ASSERT_EQ("TEST", key.allocator().name());
//   ASSERT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));
//   {
//     TestKey copy(key);

//     EXPECT_EQ("TEST", key.allocator().name());
//     EXPECT_EQ(key.allocator().name(), copy.allocator().name());
//     EXPECT_FALSE(copy.allocator().moved());
//     EXPECT_FALSE(key.allocator().movedFrom());

//     EXPECT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));
//     EXPECT_TRUE(verifyContent(copy, key.begin(), key.end()));
//     EXPECT_NE(copy.begin(), key.begin());
//   }
// }

TEST(ValueDictionaryPersistentKeyTests, ConstructByMove) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  TestAllocator allocator("TEST");
  TestKey key(BYTES, BYTES + sizeof(BYTES), allocator);

  ASSERT_EQ("TEST", key.allocator().name());
  ASSERT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));
  {
    const uint8_t* const keyBegin = key.begin();
    const uint8_t* const keyEnd = key.end();
    TestKey copy(std::move(key));

    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_TRUE(copy.allocator().moved());
    EXPECT_TRUE(key.allocator().movedFrom());

    EXPECT_TRUE(verifyContent(copy, BYTES, BYTES + sizeof(BYTES)));
    EXPECT_EQ(keyBegin, copy.begin());
    EXPECT_EQ(keyEnd, copy.end());

    EXPECT_EQ(0, key.size());
    EXPECT_EQ(key.end(), key.begin());
  }
}

TEST(ValueDictionaryPersistentKeyTests, ConstructFromValueDictionaryKey) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  ValueDictionaryKey source(BYTES, BYTES + sizeof(BYTES));
  ASSERT_EQ(BYTES, source.begin());
  ASSERT_EQ(BYTES + sizeof(BYTES), source.end());

  TestAllocator allocator("TEST");
  TestKey key(source, allocator);

  EXPECT_EQ("TEST", key.allocator().name());
  EXPECT_FALSE(key.allocator().moved());
  EXPECT_FALSE(allocator.movedFrom());
  EXPECT_TRUE(verifyContent(key, source.begin(), source.end()));
  EXPECT_NE(source.begin(), key.begin());

  EXPECT_EQ(BYTES, source.begin());
  EXPECT_EQ(BYTES + sizeof(BYTES), source.end());
}

// TEST(ValueDictionaryPersistentKeyTests, AssignByCopy) {
//   const uint8_t BYTES[] = {
//       'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
//   };
//   const uint8_t OTHER_BYTES[] = { 'c', 'o', 'w' };
//   TestAllocator allocator("TEST");
//   TestAllocator otherAllocator("OTHER");
//   TestKey key(BYTES, BYTES + sizeof(BYTES), allocator);
//   TestKey copy(OTHER_BYTES, OTHER_BYTES + sizeof(OTHER_BYTES), otherAllocator);

//   ASSERT_EQ("TEST", key.allocator().name());
//   ASSERT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));
//   ASSERT_EQ("OTHER", copy.allocator().name());
//   ASSERT_TRUE(verifyContent(copy, OTHER_BYTES,
// 			    OTHER_BYTES + sizeof(OTHER_BYTES)));

//   copy = key;

//   EXPECT_EQ("TEST", copy.allocator().name());
//   EXPECT_FALSE(copy.allocator().moved());
//   EXPECT_TRUE(verifyContent(copy, key.begin(), key.end()));

//   EXPECT_EQ("TEST", key.allocator().name());
//   EXPECT_FALSE(key.allocator().movedFrom());
//   EXPECT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));  
// }

TEST(ValueDictionaryPersistentKeyTests, AssignByMove) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  const uint8_t OTHER_BYTES[] = { 'c', 'o', 'w' };
  TestAllocator allocator("TEST");
  TestAllocator otherAllocator("OTHER");
  TestKey key(BYTES, BYTES + sizeof(BYTES), allocator);
  TestKey copy(OTHER_BYTES, OTHER_BYTES + sizeof(OTHER_BYTES), otherAllocator);
  const uint8_t* keyStart = key.begin();
  const uint8_t* keyEnd = key.end();

  ASSERT_EQ("TEST", key.allocator().name());
  ASSERT_TRUE(verifyContent(key, BYTES, BYTES + sizeof(BYTES)));
  ASSERT_EQ("OTHER", copy.allocator().name());
  ASSERT_TRUE(verifyContent(copy, OTHER_BYTES,
			    OTHER_BYTES + sizeof(OTHER_BYTES)));

  copy = std::move(key);

  EXPECT_EQ("TEST", copy.allocator().name());
  EXPECT_TRUE(copy.allocator().moved());
  EXPECT_TRUE(verifyContent(copy, BYTES, BYTES + sizeof(BYTES)));
  EXPECT_EQ(keyStart, copy.begin());
  EXPECT_EQ(keyEnd, copy.end());

  EXPECT_TRUE(key.allocator().movedFrom());
  EXPECT_EQ(0, key.size());
  EXPECT_EQ(key.end(), key.begin());
}

TEST(ValueDictionaryPersistentKeyTests, Hash) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  std::hash<TestKey> hashKey;

  EXPECT_EQ(13876786532491686417UL, hashKey(key));
}

TEST(ValueDictionaryPersistentKeyTests, Compare) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  TestKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  TestKey different(BYTES_DIFFERENT, BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  TestKey shorter(BYTES, BYTES + sizeof(BYTES) - 1);

  EXPECT_EQ(0, key.cmp(key));
  EXPECT_GT(0, shorter.cmp(key));
  EXPECT_LT(0, key.cmp(shorter));

  EXPECT_EQ(0, key.cmp(same));
  EXPECT_GT(0, key.cmp(different));
  EXPECT_LT(0, different.cmp(key));

  EXPECT_LT(0, key.cmp(TestKey(BYTES_DIFFERENT, BYTES_DIFFERENT + 2)));
  EXPECT_GT(0, TestKey(BYTES_DIFFERENT, BYTES_DIFFERENT + 2).cmp(key));
}

TEST(ValueDictionaryPersistentKeyTests, Wrap) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  ValueDictionaryKey wrapped = key.wrap();

  EXPECT_EQ(key.begin(), wrapped.begin());
  EXPECT_EQ(key.end(), wrapped.end());  
}

TEST(ValueDictionaryPersistentKeyTests, EqualityAndInequality) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  TestKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  TestKey different(BYTES_DIFFERENT, BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  TestKey shorter(BYTES, BYTES + 2);

  EXPECT_TRUE(key == key);
  EXPECT_TRUE(key == same);
  EXPECT_FALSE(key == different);
  EXPECT_FALSE(key == shorter);

  EXPECT_FALSE(key != key);
  EXPECT_FALSE(key != same);
  EXPECT_TRUE(key != different);
  EXPECT_TRUE(key != shorter);
}

TEST(ValueDictionaryPersistentKeyTests, RelationalOperators) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  TestKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  TestKey different(BYTES_DIFFERENT, BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  TestKey shorter(BYTES, BYTES + 2);

  EXPECT_FALSE(key < key);
  EXPECT_FALSE(key < same);
  EXPECT_TRUE(key < different);
  EXPECT_FALSE(different < key);
  EXPECT_FALSE(key < shorter);
  EXPECT_TRUE(shorter < key);

  EXPECT_TRUE(key <= key);
  EXPECT_TRUE(key <= same);
  EXPECT_TRUE(key <= different);
  EXPECT_FALSE(different <= key);
  EXPECT_FALSE(key <= shorter);
  EXPECT_TRUE(shorter <= key);

  EXPECT_FALSE(key > key);
  EXPECT_FALSE(key > same);
  EXPECT_FALSE(key > different);
  EXPECT_TRUE(different > key);
  EXPECT_TRUE(key > shorter);
  EXPECT_FALSE(shorter > key);
  
  EXPECT_TRUE(key >= key);
  EXPECT_TRUE(key >= same);
  EXPECT_FALSE(key >= different);
  EXPECT_TRUE(different >= key);
  EXPECT_TRUE(key >= shorter);
  EXPECT_FALSE(shorter >= key);
}

TEST(ValueDictionaryPersistentKeyTests, WriteToStream) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  TestKey key(BYTES, BYTES + sizeof(BYTES));
  std::ostringstream out;

  out << key;
  EXPECT_EQ("hello there", out.str());
}

