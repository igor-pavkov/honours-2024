short ByteVector::toShort(bool mostSignificantByteFirst) const
{
  return toNumber<unsigned short>(d->data, mostSignificantByteFirst);
}