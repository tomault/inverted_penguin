#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <gtest/gtest.h>
#include <functional>
#include <sstream>

using namespace inverted_penguin::core::dictionary::detail;

TEST(ValueDictionaryKeyTests, ConstructDefault) {
  ValueDictionaryKey key;
  EXPECT_EQ(0, key.size());
  EXPECT_EQ(key.end(), key.begin());
}

TEST(ValueDictionaryKeyTests, ConstructFromRange) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));

  EXPECT_EQ(sizeof(BYTES), key.size());
  EXPECT_EQ(BYTES, key.begin());
  EXPECT_EQ(BYTES + sizeof(BYTES), key.end());
}

TEST(ValueDictionaryKeyTests, Hash) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));
  std::hash<ValueDictionaryKey> hashKey;

  EXPECT_EQ(13876786532491686417UL, hashKey(key));
}

TEST(ValueDictionaryKeyTests, Compare) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));
  ValueDictionaryKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  ValueDictionaryKey different(BYTES_DIFFERENT,
			       BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  ValueDictionaryKey shorter(BYTES, BYTES + sizeof(BYTES) - 1);

  EXPECT_EQ(0, key.cmp(key));
  EXPECT_GT(0, shorter.cmp(key));
  EXPECT_LT(0, key.cmp(shorter));

  EXPECT_EQ(0, key.cmp(same));
  EXPECT_GT(0, key.cmp(different));
  EXPECT_LT(0, different.cmp(key));

  EXPECT_LT(0, key.cmp(ValueDictionaryKey(BYTES_DIFFERENT,
					  BYTES_DIFFERENT + 2)));
  EXPECT_GT(0, ValueDictionaryKey(BYTES_DIFFERENT,
				  BYTES_DIFFERENT + 2).cmp(key));
}

TEST(ValueDictionaryKeyTests, EqualityAndInequality) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));
  ValueDictionaryKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  ValueDictionaryKey different(BYTES_DIFFERENT,
			       BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  ValueDictionaryKey shorter(BYTES, BYTES + 2);

  EXPECT_TRUE(key == key);
  EXPECT_TRUE(key == same);
  EXPECT_FALSE(key == different);
  EXPECT_FALSE(key == shorter);

  EXPECT_FALSE(key != key);
  EXPECT_FALSE(key != same);
  EXPECT_TRUE(key != different);
  EXPECT_TRUE(key != shorter);
}

TEST(ValueDictionaryKeyTests, RelationalOperators) {
  const uint8_t BYTES[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_SAME[] = { 'a', 'b', 'c', 'd' };
  const uint8_t BYTES_DIFFERENT[] = { 'a', 'b', 'd', 'c' };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));
  ValueDictionaryKey same(BYTES_SAME, BYTES_SAME + sizeof(BYTES));
  ValueDictionaryKey different(BYTES_DIFFERENT,
			       BYTES_DIFFERENT + sizeof(BYTES_DIFFERENT));
  ValueDictionaryKey shorter(BYTES, BYTES + 2);

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

TEST(ValueDictionaryKeyTests, ArrayAccess) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));

  for (uint32_t i = 0; i < sizeof(BYTES); ++i) {
    EXPECT_EQ(BYTES[i], key[i]);
  }
}

TEST(ValueDictionaryKeyTests, WriteToStream) {
  const uint8_t BYTES[] = {
      'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e'
  };
  ValueDictionaryKey key(BYTES, BYTES + sizeof(BYTES));
  std::ostringstream out;

  out << key;
  EXPECT_EQ("hello there", out.str());
}
