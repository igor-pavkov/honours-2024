bool ByteVector::operator==(const ByteVector &v) const
{
  if(d->size != v.d->size)
    return false;

  return ::memcmp(data(), v.data(), size()) == 0;
}