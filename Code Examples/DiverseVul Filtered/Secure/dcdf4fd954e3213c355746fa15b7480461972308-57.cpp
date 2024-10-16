TagLib::uint ByteVector::checksum() const
{
  uint sum = 0;
  for(ByteVector::ConstIterator it = begin(); it != end(); ++it)
    sum = (sum << 8) ^ crcTable[((sum >> 24) & 0xff) ^ uchar(*it)];
  return sum;
}