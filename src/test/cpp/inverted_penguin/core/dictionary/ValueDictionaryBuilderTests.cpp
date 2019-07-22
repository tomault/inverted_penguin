#include <inverted_penguin/core/dictionary/ValueDictionaryBuilder.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <pistis/testing/Allocator.hpp>
#include <pistis/testing/Iterators.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <ostream>

#include <assert.h>
#include <string.h>

using namespace inverted_penguin::core::dictionary;
using inverted_penguin::core::dictionary::detail::ValueDictionaryKey;
using inverted_penguin::core::Optional;
namespace pt = pistis::testing;
namespace pti = pistis::testing::iterators;

namespace {
  typedef pt::Allocator<uint8_t> TestAllocator;
  typedef ValueDictionaryBuilder<uint32_t, TestAllocator, TestAllocator>
          DictionaryBuilder;

  // Actual size of the dictionary data, not including the padding at the
  // end needed to align to a four byte boundary
  static const size_t DICTIONARY_DATA_ACTUAL_SIZE = 133;

  static const char* TRUE_KEYS[] = {
      "apple", "big", "bigger", "biggest", "cow", "crisp", "dog",
      "penguin", "storm", "thorn"
  };
  static const uint32_t NUM_TRUE_KEYS = sizeof(TRUE_KEYS)/sizeof(const char*);
  static const char** const END_OF_TRUE_KEYS = TRUE_KEYS + NUM_TRUE_KEYS;

  static const uint32_t TRUE_VALUES[] =
      { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
  static const uint32_t NUM_TRUE_VALUES = sizeof(TRUE_VALUES)/sizeof(uint32_t);
  static const uint32_t* const END_OF_TRUE_VALUES =
      TRUE_VALUES + NUM_TRUE_VALUES;

  template <size_t n>
  static ValueDictionaryKey makeKey(const char text[n]) {
    return ValueDictionaryKey((const uint8_t*)text,
			      (const uint8_t*)text + n - 1);
  }
  
  static ValueDictionaryKey makeKey(const char* text) {
    return ValueDictionaryKey((const uint8_t*)text,
			      (const uint8_t*)text + ::strlen(text));
  }
  
  static std::vector<ValueDictionaryKey> makeTrueKeys() {
    std::vector<ValueDictionaryKey> keys;
    
    for (const char** p = TRUE_KEYS; p < END_OF_TRUE_KEYS; ++p) {
      keys.emplace_back((const uint8_t*)*p,
			(const uint8_t*)*p + ::strlen(*p));
    }

    return std::move(keys);
  }

  struct KeyValue {
    ValueDictionaryKey key;
    uint32_t value;

    KeyValue(ValueDictionaryKey k, uint32_t v): key(k), value(v) { }
    bool operator==(const KeyValue& other) const {
      return key == other.key && value == other.value;
    }
    bool operator!=(const KeyValue& other) const {
      return key != other.key && value != other.value;
    }
  };

  std::ostream& operator<<(std::ostream& out, const KeyValue& kv) {
    return out << "([" << kv.key << "], " << kv.value << ")";
  }
  
  static std::vector<KeyValue> makeTrueItems() {
    std::vector<KeyValue> items;
    assert(NUM_TRUE_KEYS == NUM_TRUE_VALUES);
    for (uint32_t i = 0; i < NUM_TRUE_KEYS; ++i) {
      ValueDictionaryKey key((const uint8_t*)TRUE_KEYS[i],
			     (const uint8_t*)TRUE_KEYS[i] +
			         ::strlen(TRUE_KEYS[i]));
      items.emplace_back(key, TRUE_VALUES[i]);
    }
    return std::move(items);
  }

  static DictionaryBuilder createTestBuilder() {
    const auto trueItems = makeTrueItems();
    DictionaryBuilder builder;
  
    for (auto kv : trueItems) {
      builder.add(kv.key.begin(), kv.key.end(), kv.value);
    }

    assert(10 == trueItems.size());
    assert(trueItems.size() == builder.size());

    return std::move(builder);
  }
}

TEST(ValueDictionaryBuilder, ConstructEmpty) {
  TestAllocator dictionaryAllocator("DICT");
  TestAllocator keyAllocator("KEY");
  DictionaryBuilder builder(dictionaryAllocator, keyAllocator);

  EXPECT_EQ("DICT", builder.dictionaryAllocator().name());
  EXPECT_EQ("KEY", builder.keyAllocator().name());
  EXPECT_EQ(0, builder.size());
  EXPECT_TRUE(builder.begin() == builder.end());
}

TEST(ValueDictionaryBuilder, Add) {
  const auto trueItems = makeTrueItems();
  DictionaryBuilder builder = createTestBuilder();
  
  for (uint32_t i = 0; i < builder.size(); ++i) {
    EXPECT_EQ(trueItems[i].key, builder.keyAt(i));
    EXPECT_EQ(trueItems[i].value, builder.valueAt(i));
  }
  
}

TEST(ValueDictionaryBuilder, ReplaceWithAdd) {
  const auto trueItems = makeTrueItems();
  DictionaryBuilder builder = createTestBuilder();
  
  ASSERT_EQ(trueItems[5].key, builder.keyAt(5));
  ASSERT_EQ(trueItems[5].value, builder.valueAt(5));

  builder.add(trueItems[5].key.begin(), trueItems[5].key.end(), 1000);
  EXPECT_EQ(trueItems[5].key, builder.keyAt(5));
  EXPECT_EQ(1000, builder.valueAt(5));
}

TEST(ValueDictionaryBuilder, ConstIterator) {
  const auto trueItems = makeTrueItems();
  DictionaryBuilder builder = createTestBuilder();

  pti::testEqualityOp(builder.end(), builder.end(), builder.begin());
  pti::testInequalityOp(builder.end(), builder.end(), builder.begin());
  {
    DictionaryBuilder::ConstIterator i = builder.cbegin();
    DictionaryBuilder::ConstIterator copy(i);
    EXPECT_TRUE(i == copy);
  }
  {
    DictionaryBuilder::ConstIterator i = builder.cbegin();
    DictionaryBuilder::ConstIterator copy = builder.cend();

    EXPECT_FALSE(i == copy);
    copy = i;
    EXPECT_TRUE(i == copy);
  }

  std::vector<KeyValue> items;
  DictionaryBuilder::ConstIterator i;
  for (i = builder.cbegin(); i != builder.cend(); ++i) {
    items.emplace_back((*i).key, (*i).value);
  }

  EXPECT_EQ(trueItems, items);

  items.clear();
  i = builder.cbegin();
  while (i != builder.cend()) {
    auto kv = *i++;
    items.emplace_back(kv.key, kv.value);
  }
  EXPECT_EQ(trueItems, items);
}

TEST(ValueDictionaryBuilder, MutableIterator) {
  const auto trueItems = makeTrueItems();
  DictionaryBuilder builder = createTestBuilder();

  pti::testEqualityOp(builder.end(), builder.end(), builder.begin());
  pti::testInequalityOp(builder.end(), builder.end(), builder.begin());
  {
    DictionaryBuilder::Iterator i = builder.begin();
    DictionaryBuilder::Iterator copy(i);
    EXPECT_TRUE(i == copy);
  }
  {
    DictionaryBuilder::Iterator i = builder.begin();
    DictionaryBuilder::Iterator copy = builder.end();

    EXPECT_FALSE(i == copy);
    copy = i;
    EXPECT_TRUE(i == copy);
  }

  std::vector<KeyValue> items;
  DictionaryBuilder::Iterator i;
  for (i = builder.begin(); i != builder.end(); ++i) {
    items.emplace_back((*i).key, (*i).value);
  }

  EXPECT_EQ(trueItems, items);

  items.clear();
  i = builder.begin();
  while (i != builder.end()) {
    auto kv = *i++;
    items.emplace_back(kv.key, kv.value);
  }
  EXPECT_EQ(trueItems, items);

  i = builder.begin();
  DictionaryBuilder::ConstIterator ci(i);
  EXPECT_TRUE(i == ci);
  EXPECT_TRUE(ci == i);
  EXPECT_FALSE(i != ci);
  EXPECT_FALSE(ci != i);

  ci = builder.end();
  EXPECT_TRUE(ci == builder.end());
}

TEST(ValueDictionaryBuilder, KeyIterator) {
  DictionaryBuilder builder = createTestBuilder();
  auto trueKeys = makeTrueKeys();
  auto makeIteratorAtStart = [&builder]() {
    return DictionaryBuilder::KeyIterator(builder.begin());
  };
  auto makeIteratorAtEnd = [&builder]() {
    return DictionaryBuilder::KeyIterator(builder.end());
  };

  pti::testForwardIterator(makeIteratorAtStart, makeIteratorAtEnd, trueKeys);
}

TEST(ValueDictionaryBuilder, ConstValueIterator) {
  DictionaryBuilder builder = createTestBuilder();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);
  auto makeIteratorAtStart = [&builder]() {
    return DictionaryBuilder::ConstValueIterator(builder.begin());
  };
  auto makeIteratorAtEnd = [&builder]() {
    return DictionaryBuilder::ConstValueIterator(builder.end());
  };

  pti::testForwardIterator(makeIteratorAtStart, makeIteratorAtEnd, trueValues);
}

TEST(ValueDictionaryBuilder, ValueIterator) {
  DictionaryBuilder builder = createTestBuilder();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);
  auto makeIteratorAtStart = [&builder]() {
    return DictionaryBuilder::ValueIterator(builder.begin());
  };
  auto makeIteratorAtEnd = [&builder]() {
    return DictionaryBuilder::ValueIterator(builder.end());
  };

  pti::testMutableForwardIterator(makeIteratorAtStart, makeIteratorAtEnd,
				  trueValues, 1000);
}

TEST(ValueDictionaryBuilder, Find) {
  DictionaryBuilder builder = createTestBuilder();
  const auto trueItems = makeTrueItems();
  const ValueDictionaryKey unknownKey = makeKey("bad_key");
  DictionaryBuilder::ConstIterator i =
      ((const DictionaryBuilder&)builder).find(trueItems[8].key.begin(),
					       trueItems[8].key.end());
  ASSERT_TRUE(i != builder.end());
  EXPECT_EQ(trueItems[8].key, (*i).key);
  EXPECT_EQ(trueItems[8].value, (*i).value);

  i = ((const DictionaryBuilder&)builder).find(unknownKey.begin(),
					       unknownKey.end());
  EXPECT_TRUE(i == builder.end());

  DictionaryBuilder::Iterator j = builder.find(trueItems[9].key.begin(),
					       trueItems[9].key.end());
  ASSERT_TRUE(j != builder.end());
  EXPECT_EQ(trueItems[9].key, (*j).key);
  EXPECT_EQ(trueItems[9].value, (*j).value);

  j = builder.find(unknownKey.begin(), unknownKey.end());
  EXPECT_TRUE(j == builder.end());
}

TEST(ValueDictionaryBuilder, Has) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::Key exists = makeKey("biggest");
  DictionaryBuilder::Key unknown = makeKey("unknown");

  EXPECT_TRUE(builder.has(exists.begin(), exists.end()));
  EXPECT_FALSE(builder.has(unknown.begin(), unknown.end()));
}

TEST(ValueDictionaryBuilder, Get) {
  DictionaryBuilder builder = createTestBuilder();
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key unknown = makeKey("unknown");
  Optional<uint32_t> value = builder.get(trueItems[0].key.begin(),
					 trueItems[0].key.end());
  ASSERT_TRUE(value.present());
  EXPECT_EQ(trueItems[0].value, value.value());

  value = builder.get(unknown.begin(), unknown.end());
  EXPECT_TRUE(value.empty());  
}

TEST(ValueDictionaryBuilder, GetOrAdd) {
  DictionaryBuilder builder;
  DictionaryBuilder::Key k1 = makeKey("cow");
  DictionaryBuilder::Key k2 = makeKey("penguin");

  EXPECT_EQ(10, builder.getOrAdd(k1.begin(), k1.end(), []() { return 10; }));
  EXPECT_EQ(20, builder.getOrAdd(k2.begin(), k2.end(), []() { return 20; }));
  EXPECT_EQ(10, builder.getOrAdd(k1.begin(), k1.end(), []() { return 30; }));
  EXPECT_EQ(2, builder.size());
  EXPECT_EQ(k1, builder.keyAt(0));
  EXPECT_EQ(k2, builder.keyAt(1));
}

TEST(ValueDictionaryBuilder, SerializedSizeInBytes) {
  DictionaryBuilder builder = createTestBuilder();
  EXPECT_EQ(DICTIONARY_DATA_ACTUAL_SIZE, builder.serializedSizeInBytes());
}

TEST(ValueDictionaryBuilder, Build) {
  DictionaryBuilder builder = createTestBuilder();
  InMemoryValueDictionary<uint32_t, TestAllocator> dictionary = builder.build();
  const auto trueItems = makeTrueItems();
  std::vector<KeyValue> items;

  ASSERT_EQ(trueItems.size(), dictionary.size());
  for (auto kv : dictionary) {
    items.emplace_back(kv.key, kv.value);
  }
  EXPECT_EQ(trueItems, items);
}

TEST(ValueDictionaryBuilderTests, PrefixMatchingOneValue) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::ConstIterator i, j;
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key prefix = makeKey("cris");

  std::tie(i, j) = builder.prefix(prefix.begin(), prefix.end());
  ASSERT_NE(i, j);
  EXPECT_EQ(trueItems[5].key, (*i).key);
  EXPECT_EQ(trueItems[5].value, (*i).value);

  ++i;
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryBuilderTests, PrefixMatchingManyValues) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::ConstIterator i, j;
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key prefix = makeKey("big");

  std::tie(i, j) = builder.prefix(prefix.begin(), prefix.end());

  ASSERT_NE(i, j);
  EXPECT_EQ(trueItems[1].key, (*i).key);
  EXPECT_EQ(trueItems[1].value, (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueItems[2].key, (*i).key);
  EXPECT_EQ(trueItems[2].value, (*i).value);

  ++i;
  ASSERT_NE(i, j);
  EXPECT_EQ(trueItems[3].key, (*i).key);
  EXPECT_EQ(trueItems[3].value, (*i).value);

  ++i;
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryBuilderTests, PrefixMatchingNoValues) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::ConstIterator i, j;
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key prefix = makeKey("cu");

  std::tie(i, j) = builder.prefix(prefix.begin(), prefix.end());
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryBuilderTests, PrefixMatchingNoValuesKeyTooLong) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::ConstIterator i, j;
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key prefix = makeKey("doggi");

  std::tie(i, j) = builder.prefix(prefix.begin(), prefix.end());
  EXPECT_EQ(i, j);
}

TEST(ValueDictionaryBuilderTests, PrefixMatchingNoValuesIncorrectSuffix) {
  DictionaryBuilder builder = createTestBuilder();
  DictionaryBuilder::ConstIterator i, j;
  const auto trueItems = makeTrueItems();
  DictionaryBuilder::Key prefix = makeKey("stou");

  std::tie(i, j) = builder.prefix(prefix.begin(), prefix.end());
  EXPECT_EQ(i, j);
}
