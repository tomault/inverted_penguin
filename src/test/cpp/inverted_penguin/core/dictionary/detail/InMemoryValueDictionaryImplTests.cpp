#include <inverted_penguin/core/dictionary/detail/InMemoryValueDictionaryImpl.hpp>
#include <pistis/testing/Allocator.hpp>
#include <gtest/gtest.h>

using namespace inverted_penguin::core::dictionary::detail;
namespace pt = pistis::testing;

namespace {
  typedef pt::Allocator<uint8_t> DictionaryAllocator;
  typedef InMemoryValueDictionaryImpl<uint32_t, DictionaryAllocator>
          DictImpl;

  // TODO: Put this into a common test artifact
  // Keys and their corresponding values:
  //   apple     10
  //   big       20
  //   bigger    30
  //   biggest   40
  //   cow       50
  //   crisp     60
  //   dog       70
  //   penguin   80
  //   storm     90
  //   thorn    100
  alignas(4) static const uint8_t DICTIONARY_DATA[] = {
      0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
      0x0d, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00,
      0x1c, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00,
      0x2b, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
      0x14, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
      0x32, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00,
      0x50, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
      0x61, 0x70, 0x70, 0x6c, 0x65, 0x62, 0x69, 0x67, 0x62, 0x69, 0x67, 0x67,
      0x65, 0x72, 0x62, 0x69, 0x67, 0x67, 0x65, 0x73, 0x74, 0x63, 0x6f, 0x77,
      0x63, 0x72, 0x69, 0x73, 0x70, 0x64, 0x6f, 0x67, 0x70, 0x65, 0x6e, 0x67,
      0x75, 0x69, 0x6e, 0x73, 0x74, 0x6f, 0x72, 0x6d, 0x74, 0x68, 0x6f, 0x72,
      0x6e, 0x00, 0x00, 0x00,
  };

  alignas(4) static const uint8_t EMPTY_DICTIONARY[] = {
      0x00, 0x00, 0x00, 0x00
  };

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

  static std::vector<DictImpl::Key> makeTrueKeys() {
    std::vector<DictImpl::Key> keys;
    
    for (const char** p = TRUE_KEYS; p < END_OF_TRUE_KEYS; ++p) {
      keys.emplace_back((const uint8_t*)*p,
			(const uint8_t*)*p + ::strlen(*p));
    }

    return std::move(keys);
  }

}

TEST(InMemoryValueDictionaryTests, Construct) {
  DictionaryAllocator allocator("TEST");
  uint8_t* const dictData = allocator.allocate(sizeof(DICTIONARY_DATA));
  ::memcpy(dictData, DICTIONARY_DATA, sizeof(DICTIONARY_DATA));

  DictImpl dictionary(dictData, sizeof(DICTIONARY_DATA), allocator);
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);

  EXPECT_EQ(dictData, dictionary.base());
  EXPECT_EQ(sizeof(DICTIONARY_DATA), dictionary.sizeInBytes());
  EXPECT_EQ("TEST", dictionary.allocator().name());
  EXPECT_FALSE(dictionary.allocator().moved());

  ASSERT_EQ(trueKeys.size(), dictionary.size());
  ASSERT_EQ(NUM_TRUE_VALUES, dictionary.size());

  std::vector<DictImpl::Key> keys;
  std::vector<uint32_t> values;
  for (auto kv : dictionary) {
    keys.push_back(kv.key);
    values.push_back(kv.value);
  }

  EXPECT_EQ(trueKeys, keys);
  EXPECT_EQ(trueValues, values);
}

TEST(InMemoryValueDictionaryTests, ConstructByMove) {
  DictionaryAllocator allocator("TEST");
  uint8_t* const dictData = allocator.allocate(sizeof(DICTIONARY_DATA));
  ::memcpy(dictData, DICTIONARY_DATA, sizeof(DICTIONARY_DATA));

  DictImpl dictionary(dictData, sizeof(DICTIONARY_DATA), allocator);
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);

  ASSERT_EQ(dictData, dictionary.base());
  ASSERT_EQ(sizeof(DICTIONARY_DATA), dictionary.sizeInBytes());
  ASSERT_EQ("TEST", dictionary.allocator().name());
  ASSERT_FALSE(dictionary.allocator().moved());
  ASSERT_EQ(trueKeys.size(), dictionary.size());
  ASSERT_EQ(NUM_TRUE_VALUES, dictionary.size());
  {
    DictImpl copy(std::move(dictionary));

    EXPECT_EQ(dictData, copy.base());
    EXPECT_EQ(sizeof(DICTIONARY_DATA), copy.sizeInBytes());
    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_TRUE(copy.allocator().moved());

    std::vector<DictImpl::Key> keys;
    std::vector<uint32_t> values;
    for (auto kv : copy) {
      keys.push_back(kv.key);
      values.push_back(kv.value);
    }

    EXPECT_EQ(trueKeys, keys);
    EXPECT_EQ(trueValues, values);

    EXPECT_EQ((const uint8_t*)0, dictionary.base());
    EXPECT_EQ(0, dictionary.sizeInBytes());
  }
}

TEST(InMemoryValueDictionaryTests, AssignByMove) {
  DictionaryAllocator allocator("TEST");
  uint8_t* const dictData = allocator.allocate(sizeof(DICTIONARY_DATA));
  ::memcpy(dictData, DICTIONARY_DATA, sizeof(DICTIONARY_DATA));

  DictImpl dictionary(dictData, sizeof(DICTIONARY_DATA), allocator);

  DictionaryAllocator otherAllocator("OTHER");
  uint8_t* const emptyData = otherAllocator.allocate(sizeof(EMPTY_DICTIONARY));
  ::memcpy(emptyData, EMPTY_DICTIONARY, sizeof(EMPTY_DICTIONARY));
  
  DictImpl copy(emptyData, sizeof(EMPTY_DICTIONARY), otherAllocator);
  auto trueKeys = makeTrueKeys();
  std::vector<uint32_t> trueValues(TRUE_VALUES, END_OF_TRUE_VALUES);

  ASSERT_EQ(dictData, dictionary.base());
  ASSERT_EQ(sizeof(DICTIONARY_DATA), dictionary.sizeInBytes());
  ASSERT_EQ("TEST", dictionary.allocator().name());
  ASSERT_FALSE(dictionary.allocator().moved());

  ASSERT_EQ(emptyData, copy.base());
  ASSERT_EQ(sizeof(EMPTY_DICTIONARY), copy.sizeInBytes());
  ASSERT_EQ("OTHER", copy.allocator().name());
  ASSERT_FALSE(copy.allocator().moved());  
  
  ASSERT_EQ(trueKeys.size(), dictionary.size());
  ASSERT_EQ(NUM_TRUE_VALUES, dictionary.size());
  {
    copy = std::move(dictionary);

    EXPECT_EQ(dictData, copy.base());
    EXPECT_EQ(sizeof(DICTIONARY_DATA), copy.sizeInBytes());
    EXPECT_EQ("TEST", copy.allocator().name());
    EXPECT_TRUE(copy.allocator().moved());

    std::vector<DictImpl::Key> keys;
    std::vector<uint32_t> values;
    for (auto kv : copy) {
      keys.push_back(kv.key);
      values.push_back(kv.value);
    }

    EXPECT_EQ(trueKeys, keys);
    EXPECT_EQ(trueValues, values);

    EXPECT_EQ((const uint8_t*)0, dictionary.base());
    EXPECT_EQ(0, dictionary.sizeInBytes());
  }
}
