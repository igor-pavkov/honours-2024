ByteVector &ByteVector::append(const ByteVector &v)
{
  if(v.d->size == 0)
    return *this; // Simply return if appending nothing.

  detach();

  uint originalSize = d->size;
  resize(d->size + v.d->size);
  ::memcpy(DATA(d) + originalSize, DATA(v.d), v.size());

  return *this;
}