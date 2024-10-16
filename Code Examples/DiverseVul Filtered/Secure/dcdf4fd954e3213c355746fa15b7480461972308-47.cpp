ByteVector::ByteVector(char c)
{
  d = new ByteVectorPrivate;
  d->data.push_back(c);
  d->size = 1;
}