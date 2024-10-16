ByteVector ByteVector::fromUInt(uint value, bool mostSignificantByteFirst)
{
  return fromNumber<uint>(value, mostSignificantByteFirst);
}