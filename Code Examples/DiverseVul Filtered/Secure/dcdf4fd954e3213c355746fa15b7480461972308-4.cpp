ByteVector::ByteVector(const ByteVector &v) : d(v.d)
{
  d->ref();
}