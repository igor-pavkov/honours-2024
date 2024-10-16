int ByteVector::find(const ByteVector &pattern, uint offset, int byteAlign) const
{
  return vectorFind<ByteVector>(*this, pattern, offset, byteAlign);
}