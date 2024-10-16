uint64 EbmlElement::ElementSize(bool bWithDefault) const
{
  if (!bWithDefault && IsDefaultValue())
    return 0; // won't be saved
  return Size + EBML_ID_LENGTH((const EbmlId&)*this) + CodedSizeLength(Size, SizeLength, bSizeIsFinite);
}