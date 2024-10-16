char &ByteVector::operator[](int index)
{
  detach();

  return d->data[index];
}