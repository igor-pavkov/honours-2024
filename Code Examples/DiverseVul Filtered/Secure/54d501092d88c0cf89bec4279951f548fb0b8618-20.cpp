static int zipfileAppendEntry(
  ZipfileTab *pTab,
  ZipfileEntry *pEntry,
  const u8 *pData,
  int nData
){
  u8 *aBuf = pTab->aBuffer;
  int nBuf;
  int rc;

  nBuf = zipfileSerializeLFH(pEntry, aBuf);
  rc = zipfileAppendData(pTab, aBuf, nBuf);
  if( rc==SQLITE_OK ){
    pEntry->iDataOff = pTab->szCurrent;
    rc = zipfileAppendData(pTab, pData, nData);
  }

  return rc;
}