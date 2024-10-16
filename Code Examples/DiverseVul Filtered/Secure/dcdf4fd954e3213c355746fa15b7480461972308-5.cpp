char ByteVector::at(uint index) const
{
  return index < size() ? d->data[index] : 0;
}