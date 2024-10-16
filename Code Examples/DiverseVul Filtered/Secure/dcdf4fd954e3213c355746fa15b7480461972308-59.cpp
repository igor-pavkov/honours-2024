ByteVector ByteVector::toHex() const
{
  ByteVector encoded(size() * 2);

  uint j = 0;
  for(uint i = 0; i < size(); i++) {
    unsigned char c = d->data[i];
    encoded[j++] = hexTable[(c >> 4) & 0x0F];
    encoded[j++] = hexTable[(c     ) & 0x0F];
  }

  return encoded;
}