bool EbmlMaster::VerifyChecksum() const
{
  if (!bChecksumUsed)
    return true;

  EbmlCrc32 aChecksum;
  /// \todo remove the Checksum if it's in the list
  /// \todo find another way when not all default values are saved or (unknown from the reader !!!)
  MemIOCallback TmpBuf(GetSize() - 6);
  for (size_t Index = 0; Index < ElementList.size(); Index++) {
    (ElementList[Index])->Render(TmpBuf, true, false, true);
  }
  aChecksum.FillCRC32(TmpBuf.GetDataBuffer(), TmpBuf.GetDataBufferSize());
  return (aChecksum.GetCrc32() == Checksum.GetCrc32());
}