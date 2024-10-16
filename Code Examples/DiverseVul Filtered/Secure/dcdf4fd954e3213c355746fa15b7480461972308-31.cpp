ByteVector &ByteVector::clear()
{
  detach();
  d->data.clear();
  d->size = 0;

  return *this;
}