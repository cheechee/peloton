//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// boolean_value.h
//
// Identification: src/backend/common/timestamp_value.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "common/timestamp_value.h"
#include "common/boolean_value.h"
#include "common/varlen_value.h"

namespace peloton {
namespace common {

TimestampType::TimestampType()
  : Type(Type::TIMESTAMP) {
}

Value *TimestampType::CompareEquals(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (left.IsNull() || right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<uint64_t>() == right.GetAs<uint64_t>());
}

Value *TimestampType::CompareNotEquals(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<uint64_t>() != right.GetAs<uint64_t>());
}

Value *TimestampType::CompareLessThan(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (left.IsNull() || right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<uint64_t>() < right.GetAs<uint64_t>());
}

Value *TimestampType::CompareLessThanEquals(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (left.IsNull() || right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<uint64_t>() <= right.GetAs<uint64_t>());
}

Value *TimestampType::CompareGreaterThan(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (left.IsNull() || right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<int64_t>() > right.GetAs<int64_t>());
}

Value *TimestampType::CompareGreaterThanEquals(const Value& left, const Value &right) const {
  left.CheckComparable(right);
  if (left.IsNull() || right.IsNull())
    return new Value(Type::BOOLEAN, PELOTON_BOOLEAN_NULL);
  return new Value(Type::BOOLEAN, left.GetAs<uint64_t>() >= right.GetAs<uint64_t>());
}

// Debug
std::string TimestampType::ToString(const Value& val) const {
  if (val.IsNull())
    return "timestamp_null";
  uint64_t tm = val.value_.timestamp;
  uint32_t micro = tm % 1000000;
  tm /= 1000000;
  uint32_t second = tm % 100000;
  uint16_t sec = second % 60;
  second /= 60;
  uint16_t min = second % 60;
  second /= 60;
  uint16_t hour = second % 24;
  tm /= 100000;
  uint16_t year = tm % 10000;
  tm /= 10000;
  int tz = tm % 27;
  tz -= 12;
  tm /= 27;
  uint16_t day = tm % 32;
  tm /= 32;
  uint16_t month = tm;
  char str[30];
  char zone[5];
  sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
    year, month, day, hour, min, sec, micro);
  if(tz >= 0) {
    str[26] = '+';
  }
  else
    str[26] = '-';
  if (tz < 0)
    tz = -tz;
  sprintf(zone, "%02d", tz);
  str[27] = 0;
  return std::string(std::string(str) + std::string(zone));
}

// Compute a hash value
size_t TimestampType::Hash(const Value& val) const {
  return std::hash<uint64_t>{}(val.value_.timestamp);
}

void TimestampType::HashCombine(const Value& val, size_t &seed) const {
  hash_combine<int64_t>(seed, val.value_.timestamp);
}

void TimestampType::SerializeTo(const Value& val, SerializeOutput &out) const {
  out.WriteLong(val.value_.timestamp);
}

void TimestampType::SerializeTo(const Value& val, char *storage, bool inlined UNUSED_ATTRIBUTE,
                                 VarlenPool *pool UNUSED_ATTRIBUTE) const {
  *reinterpret_cast<uint64_t *>(storage) = val.value_.timestamp;
}

// Create a copy of this value
Value *TimestampType::Copy(const Value& val) const {
  return new Value(Type::TIMESTAMP, val.value_.timestamp);
}

Value *TimestampType::CastAs(const Value& val, const Type::TypeId type_id) const {
  switch (type_id) {
    case Type::TIMESTAMP:
      return val.Copy();
    case Type::VARCHAR:
      if (val.IsNull())
        return new Value(Type::VARCHAR, nullptr, 0);
      return new Value(Type::VARCHAR, val.ToString());
    default:
      break;
  }
  throw Exception("TIMESTAMP is not coercable to "
      + Type::GetInstance(type_id).ToString());
}

}  // namespace peloton
}  // namespace common
