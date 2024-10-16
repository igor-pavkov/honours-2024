bool ByteVector::operator<(const ByteVector &v) const
{
  int result = ::memcmp(data(), v.data(), d->size < v.d->size ? d->size : v.d->size);

  if(result != 0)
    return result < 0;
  else
    return size() < v.size();
}