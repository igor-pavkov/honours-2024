filepos_t EbmlMaster::RenderData(IOCallback & output, bool bForceRender, bool bWithDefault)
{
  filepos_t Result = 0;
  size_t Index;

  if (!bForceRender) {
    assert(CheckMandatory());
  }

  if (!bChecksumUsed) { // old school
    for (Index = 0; Index < ElementList.size(); Index++) {
      if (!bWithDefault && (ElementList[Index])->IsDefaultValue())
        continue;
      Result += (ElementList[Index])->Render(output, bWithDefault, false ,bForceRender);
    }
  } else { // new school
    MemIOCallback TmpBuf(GetSize() - 6);
    for (Index = 0; Index < ElementList.size(); Index++) {
      if (!bWithDefault && (ElementList[Index])->IsDefaultValue())
        continue;
      (ElementList[Index])->Render(TmpBuf, bWithDefault, false ,bForceRender);
    }
    Checksum.FillCRC32(TmpBuf.GetDataBuffer(), TmpBuf.GetDataBufferSize());
    Result += Checksum.Render(output, true, false ,bForceRender);
    output.writeFully(TmpBuf.GetDataBuffer(), TmpBuf.GetDataBufferSize());
    Result += TmpBuf.GetDataBufferSize();
  }

  return Result;
}