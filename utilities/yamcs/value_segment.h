
#pragma once

namespace ROCKSDB_NAMESPACE {
namespace yamcs {

#include <rocksdb/slice.h>
#include <rocksdb/status.h>

class ValueSegment {
 public:
  virtual ~ValueSegment() =
      default;

  virtual void WriteTo(std::string &slice) = 0;
  virtual void MergeFrom(const rocksdb::Slice &slice, size_t &pos) = 0;
  virtual size_t MaxSerializedSize() = 0;

  // Returns the number of values of the serialized segment starting at pos
  // (the position passed to MergeFrom), read from its header without decoding
  // the values. Returns 0 if the segment type does not support it or the
  // header cannot be read.
  virtual size_t PeekNumValues(const rocksdb::Slice & /*slice*/,
                               size_t /*pos*/) {
    return 0;
  }

  // Reserves room for n values in total, such that merging segments with that
  // many values in total does not need to reallocate.
  virtual void Reserve(size_t /*n*/) {}

  rocksdb::Status& Status() {
    return status;
  }
  
  protected:
  rocksdb::Status status;
    
};
}  // namespace yamcs
}  // namespace ROCKSDB_NAMESPACE