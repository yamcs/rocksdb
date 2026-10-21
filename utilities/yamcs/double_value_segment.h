#pragma once

#include <vector>
#include <cstdint>
#include <rocksdb/slice.h>
#include <rocksdb/status.h>

#include "value_segment.h"

namespace ROCKSDB_NAMESPACE {
namespace yamcs {

class DoubleValueSegment : public ValueSegment {
public:
    static const uint8_t SUBFORMAT_ID_RAW = 0;

    DoubleValueSegment(const Slice &slice, size_t &pos);
    
    void WriteTo(std::string& slice) override;
    void MergeFrom(const rocksdb::Slice& slice, size_t& pos) override;
    size_t MaxSerializedSize() override {
         return 4 + 8 * values.size();
    }
    size_t PeekNumValues(const rocksdb::Slice &slice, size_t pos) override;
    void Reserve(size_t n) override { values.reserve(n); }

private:
    void parseRaw(const rocksdb::Slice& slice, int n);
    std::vector<double> values;
};

} // namespace yamcs
} // namespace ROCKSDB_NAMESPACE

