bool EbmlElement::ForceSize(uint64 NewSize)
{
  if (bSizeIsFinite) {
    return false;
  }

  int OldSizeLen = CodedSizeLength(Size, SizeLength, bSizeIsFinite);
  uint64 OldSize = Size;

  Size = NewSize;

  if (CodedSizeLength(Size, SizeLength, bSizeIsFinite) == OldSizeLen) {
    bSizeIsFinite = true;
    return true;
  }
  Size = OldSize;

  return false;
}