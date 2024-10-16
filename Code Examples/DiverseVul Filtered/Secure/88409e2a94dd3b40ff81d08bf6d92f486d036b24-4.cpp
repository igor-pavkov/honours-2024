filepos_t EbmlMaster::WriteHead(IOCallback & output, int nSizeLength, bool bWithDefault)
{
  SetSizeLength(nSizeLength);
  return RenderHead(output, false, bWithDefault);
}