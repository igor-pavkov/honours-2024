int CodedSizeLength(uint64 Length, unsigned int SizeLength, bool bSizeFinite)
{
  unsigned int CodedSize;
  if (bSizeFinite) {
    // prepare the head of the size (000...01xxxxxx)
    // optimal size
    if (Length < 127) // 2^7 - 1
      CodedSize = 1;
    else if (Length < 16383) // 2^14 - 1
      CodedSize = 2;
    else if (Length < 2097151L) // 2^21 - 1
      CodedSize = 3;
    else if (Length < 268435455L) // 2^28 - 1
      CodedSize = 4;
    else CodedSize = 5;
  } else {
    if (Length <= 127) // 2^7 - 1
      CodedSize = 1;
    else if (Length <= 16383) // 2^14 - 1
      CodedSize = 2;
    else if (Length <= 2097151L) // 2^21 - 1
      CodedSize = 3;
    else if (Length <= 268435455L) // 2^28 - 1
      CodedSize = 4;
    else CodedSize = 5;
  }

  if (SizeLength > 0 && CodedSize < SizeLength) {
    // defined size
    CodedSize = SizeLength;
  }

  return CodedSize;
}