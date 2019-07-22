#include <inverted_penguin/core/dictionary/detail/ValueDictionaryWriter.hpp>
#include <inverted_penguin/core/dictionary/detail/ValueDictionaryKey.hpp>
#include <gtest/gtest.h>

#include <assert.h>
#include <string.h>

using namespace inverted_penguin::core::dictionary::detail;

namespace {
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

  // Actual size of the dictionary data, not including the padding at the
  // end needed to align to a four byte boundary
  static const size_t DICTIONARY_DATA_ACTUAL_SIZE = sizeof(DICTIONARY_DATA) - 3;
  
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
  };
  
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
  
}

TEST(ValueDictionaryWriterTests, Write) {
  ValueDictionaryWriter<uint32_t> writer;
  const std::vector<KeyValue> items = makeTrueItems();
  const size_t sizeOfData = writer.dataSize(items.size(), items.begin());

  ASSERT_EQ(10, items.size());
  ASSERT_EQ(DICTIONARY_DATA_ACTUAL_SIZE, sizeOfData);

  std::unique_ptr<uint8_t[]> buffer(new uint8_t[sizeOfData]);
  uint32_t bufferIndex = 0;
  auto writeBytes = [&buffer, &bufferIndex, &sizeOfData](const void* data,
							 size_t n) {
    assert((bufferIndex + n) <= sizeOfData);
    ::memcpy(buffer.get() + bufferIndex, data, n);
    bufferIndex += n;
    return n;
  };

  const size_t numWritten = writer.write(items.size(), items.begin(),
					 writeBytes);
  EXPECT_EQ(DICTIONARY_DATA_ACTUAL_SIZE, numWritten);
  EXPECT_FALSE(::memcmp(buffer.get(), DICTIONARY_DATA,
			DICTIONARY_DATA_ACTUAL_SIZE));
}
