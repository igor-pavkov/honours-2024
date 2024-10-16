ByteVector &ByteVector::resize(uint size, char padding)
{
  if(d->size < size) {
    d->data.reserve(size);
    d->data.insert(d->data.end(), size - d->size, padding);
  }
  else
    d->data.erase(d->data.begin() + size, d->data.end());

  d->size = size;

  return *this;
}