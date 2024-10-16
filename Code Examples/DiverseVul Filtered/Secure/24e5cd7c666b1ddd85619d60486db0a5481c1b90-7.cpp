int CodedSizeLengthSigned(int64 Length, unsigned int SizeLength)
{
  unsigned int CodedSize;
  // prepare the head of the size (000...01xxxxxx)
  // optimal size
  if (Length > -64 && Length < 64) // 2^6
    CodedSize = 1;
  else if (Length > -8192 && Length < 8192) // 2^13
    CodedSize = 2;
  else if (Length > -1048576L && Length < 1048576L) // 2^20
    CodedSize = 3;
  else if (Length > -134217728L && Length < 134217728L) // 2^27
    CodedSize = 4;
  else CodedSize = 5;

  if (SizeLength > 0 && CodedSize < SizeLength) {
    // defined size
    CodedSize = SizeLength;
  }

  return CodedSize;
}