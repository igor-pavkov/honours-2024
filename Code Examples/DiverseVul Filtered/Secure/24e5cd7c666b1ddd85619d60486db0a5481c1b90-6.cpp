int CodedValueLength(uint64 Length, int CodedSize, binary * OutBuffer)
{
  int _SizeMask = 0xFF;
  OutBuffer[0] = 1 << (8 - CodedSize);
  for (int i=1; i<CodedSize; i++) {
    OutBuffer[CodedSize-i] = Length & 0xFF;
    Length >>= 8;
    _SizeMask >>= 1;
  }
  // first one use a OR with the "EBML size head"
  OutBuffer[0] |= Length & 0xFF & _SizeMask;
  return CodedSize;
}