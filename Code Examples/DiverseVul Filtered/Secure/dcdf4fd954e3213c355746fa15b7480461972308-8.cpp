ByteVector ByteVector::fromLongLong(long long value, bool mostSignificantByteFirst)
{
  return fromNumber<long long>(value, mostSignificantByteFirst);
}