ByteVector &ByteVector::operator=(const ByteVector &v)
{
  if(&v == this)
    return *this;

  if(d->deref())
    delete d;

  d = v.d;
  d->ref();
  return *this;
}