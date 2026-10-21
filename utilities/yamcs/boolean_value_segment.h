#pragma once

#include <rocksdb/slice.h>
#include <rocksdb/status.h>

#include <cstdint>
#include <vector>

#include "boolean_array.h"
#include "value_segment.h"

namespace ROCKSDB_NAMESPACE {
namespace yamcs {

class BooleanValueSegment : public ValueSegment {
 public:
 BooleanValueSegment(const Slice& slice, size_t& pos);

  void WriteTo(std::string& slice) override;
  void MergeFrom(const rocksdb::Slice& slice, size_t& pos) override;
  size_t MaxSerializedSize() override {
    // 4 bytes max for the segment size
    // 4 bytes max for the long array length
    // 8 bytes for each 64 bits, rounded up
    return 16 + ba.size() / 8;
  }

 private:
  void parseRaw(const rocksdb::Slice& slice, int n);

  BooleanArray ba;
};

}  // namespace yamcs
}  // namespace ROCKSDB_NAMESPACE
