#pragma once

#include <rocksdb/slice.h>
#include <rocksdb/status.h>

#include <cstdint>
#include <vector>

#include "value_segment.h"

namespace ROCKSDB_NAMESPACE {
namespace yamcs {

class FloatValueSegment : public ValueSegment {
 public:
  static const uint8_t SUBFORMAT_ID_RAW = 0;
  static const uint8_t SUBFORMAT_ID_COMPRESSED = 1;

  FloatValueSegment(const Slice &slice, size_t &pos);
  
  void WriteTo(std::string& slice) override;
  void MergeFrom(const rocksdb::Slice& slice, size_t& pos) override;
  size_t MaxSerializedSize() override { return 5 + 4 * values.size() + 1; }
  size_t PeekNumValues(const rocksdb::Slice &slice, size_t pos) override;
  void Reserve(size_t n) override { values.reserve(n); }

 private:
  void parseRaw(const rocksdb::Slice& slice, int n);
  void writeRaw(std::string& slice);
  void writeCompressed(std::string& buf);
  std::vector<float> values;
};

}  // namespace yamcs
}  // namespace ROCKSDB_NAMESPACE
