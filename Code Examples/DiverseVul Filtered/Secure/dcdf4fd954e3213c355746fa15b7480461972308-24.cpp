int ByteVector::rfind(const ByteVector &pattern, uint offset, int byteAlign) const
{
  // Ok, this is a little goofy, but pretty cool after it sinks in.  Instead of
  // reversing the find method's Boyer-Moore search algorithm I created a "mirror"
  // for a ByteVector to reverse the behavior of the accessors.

  ByteVectorMirror v(*this);
  ByteVectorMirror p(pattern);

  return v.find(p, offset, byteAlign);
}