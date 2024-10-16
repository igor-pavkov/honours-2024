ByteVector::ByteVector(uint size, char value)
{
  d = new ByteVectorPrivate(size, value);
}