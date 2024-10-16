static int zipfileLoadDirectory(ZipfileTab *pTab, const u8 *aBlob, int nBlob){
  ZipfileEOCD eocd;
  int rc;
  int i;
  i64 iOff;

  rc = zipfileReadEOCD(pTab, aBlob, nBlob, pTab->pWriteFd, &eocd);
  iOff = eocd.iOffset;
  for(i=0; rc==SQLITE_OK && i<eocd.nEntry; i++){
    ZipfileEntry *pNew = 0;
    rc = zipfileGetEntry(pTab, aBlob, nBlob, pTab->pWriteFd, iOff, &pNew);

    if( rc==SQLITE_OK ){
      zipfileAddEntry(pTab, 0, pNew);
      iOff += ZIPFILE_CDS_FIXED_SZ;
      iOff += (int)pNew->cds.nExtra + pNew->cds.nFile + pNew->cds.nComment;
    }
  }
  return rc;
}