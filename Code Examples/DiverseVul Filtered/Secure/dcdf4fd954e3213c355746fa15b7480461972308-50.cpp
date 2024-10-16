ByteVector ByteVector::fromShort(short value, bool mostSignificantByteFirst)
{
  return fromNumber<short>(value, mostSignificantByteFirst);
}