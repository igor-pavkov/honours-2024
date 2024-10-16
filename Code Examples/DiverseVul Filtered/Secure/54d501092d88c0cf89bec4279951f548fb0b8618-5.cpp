static int zipfileSerializeCDS(ZipfileEntry *pEntry, u8 *aBuf){
  u8 *a = aBuf;
  ZipfileCDS *pCDS = &pEntry->cds;

  if( pEntry->aExtra==0 ){
    pCDS->nExtra = 9;
  }

  zipfileWrite32(a, ZIPFILE_SIGNATURE_CDS);
  zipfileWrite16(a, pCDS->iVersionMadeBy);
  zipfileWrite16(a, pCDS->iVersionExtract);
  zipfileWrite16(a, pCDS->flags);
  zipfileWrite16(a, pCDS->iCompression);
  zipfileWrite16(a, pCDS->mTime);
  zipfileWrite16(a, pCDS->mDate);
  zipfileWrite32(a, pCDS->crc32);
  zipfileWrite32(a, pCDS->szCompressed);
  zipfileWrite32(a, pCDS->szUncompressed);
  assert( a==&aBuf[ZIPFILE_CDS_NFILE_OFF] );
  zipfileWrite16(a, pCDS->nFile);
  zipfileWrite16(a, pCDS->nExtra);
  zipfileWrite16(a, pCDS->nComment);
  zipfileWrite16(a, pCDS->iDiskStart);
  zipfileWrite16(a, pCDS->iInternalAttr);
  zipfileWrite32(a, pCDS->iExternalAttr);
  zipfileWrite32(a, pCDS->iOffset);

  memcpy(a, pCDS->zFile, pCDS->nFile);
  a += pCDS->nFile;

  if( pEntry->aExtra ){
    int n = (int)pCDS->nExtra + (int)pCDS->nComment;
    memcpy(a, pEntry->aExtra, n);
    a += n;
  }else{
    assert( pCDS->nExtra==9 );
    zipfileWrite16(a, ZIPFILE_EXTRA_TIMESTAMP);
    zipfileWrite16(a, 5);
    *a++ = 0x01;
    zipfileWrite32(a, pEntry->mUnixTime);
  }

  return a-aBuf;
}