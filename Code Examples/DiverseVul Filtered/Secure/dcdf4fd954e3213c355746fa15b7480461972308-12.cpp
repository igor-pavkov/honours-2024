char *ByteVector::data()
{
  detach();
  return size() > 0 ? DATA(d) : 0;
}