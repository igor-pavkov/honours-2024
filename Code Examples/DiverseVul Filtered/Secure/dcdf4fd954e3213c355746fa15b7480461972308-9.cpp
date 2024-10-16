const char &ByteVector::operator[](int index) const
{
  return d->data[index];
}