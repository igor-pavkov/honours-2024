int64 ReadCodedSizeSignedValue(const binary * InBuffer, uint32 & BufferSize, uint64 & SizeUnknown)
{
  int64 Result = ReadCodedSizeValue(InBuffer, BufferSize, SizeUnknown);

  if (BufferSize != 0) {
    switch (BufferSize) {
      case 1:
        Result -= 63;
        break;
      case 2:
        Result -= 8191;
        break;
      case 3:
        Result -= 1048575L;
        break;
      case 4:
        Result -= 134217727L;
        break;
    }
  }

  return Result;
}