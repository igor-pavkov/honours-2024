long long ByteVector::toLongLong(bool mostSignificantByteFirst) const
{
  return toNumber<unsigned long long>(d->data, mostSignificantByteFirst);
}