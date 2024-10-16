unsigned short ByteVector::toUShort(bool mostSignificantByteFirst) const
{
  return toNumber<unsigned short>(d->data, mostSignificantByteFirst);
}