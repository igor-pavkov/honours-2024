filepos_t EbmlElement::MakeRenderHead(IOCallback & output, bool bKeepPosition)
{
  binary FinalHead[4+8]; // Class D + 64 bits coded size
  unsigned int FinalHeadSize;

  FinalHeadSize = EBML_ID_LENGTH((const EbmlId&)*this);
  EbmlId(*this).Fill(FinalHead);

  int CodedSize = CodedSizeLength(Size, SizeLength, bSizeIsFinite);
  CodedValueLength(Size, CodedSize, &FinalHead[FinalHeadSize]);
  FinalHeadSize += CodedSize;

  output.writeFully(FinalHead, FinalHeadSize);
  if (!bKeepPosition) {
    ElementPosition = output.getFilePointer() - FinalHeadSize;
    SizePosition = ElementPosition + EBML_ID_LENGTH((const EbmlId&)*this);
  }

  return FinalHeadSize;
}