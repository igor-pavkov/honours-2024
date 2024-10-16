static int zipfileReadLFH(
  u8 *aBuffer,
  ZipfileLFH *pLFH
){
  u8 *aRead = aBuffer;
  int rc = SQLITE_OK;

  u32 sig = zipfileRead32(aRead);
  if( sig!=ZIPFILE_SIGNATURE_LFH ){
    rc = SQLITE_ERROR;
  }else{
    pLFH->iVersionExtract = zipfileRead16(aRead);
    pLFH->flags = zipfileRead16(aRead);
    pLFH->iCompression = zipfileRead16(aRead);
    pLFH->mTime = zipfileRead16(aRead);
    pLFH->mDate = zipfileRead16(aRead);
    pLFH->crc32 = zipfileRead32(aRead);
    pLFH->szCompressed = zipfileRead32(aRead);
    pLFH->szUncompressed = zipfileRead32(aRead);
    pLFH->nFile = zipfileRead16(aRead);
    pLFH->nExtra = zipfileRead16(aRead);
  }
  return rc;
}