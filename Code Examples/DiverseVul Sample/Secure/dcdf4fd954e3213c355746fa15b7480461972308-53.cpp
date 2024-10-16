TagLib::uint ByteVector::toUInt(bool mostSignificantByteFirst) const
{
  return toNumber<uint>(d->data, mostSignificantByteFirst);
}