const char *ByteVector::data() const
{
  return size() > 0 ? DATA(d) : 0;
}