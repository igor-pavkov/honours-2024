static int zipfileSerializeLFH(ZipfileEntry *pEntry, u8 *aBuf){
  ZipfileCDS *pCds = &pEntry->cds;
  u8 *a = aBuf;

  pCds->nExtra = 9;

  /* Write the LFH itself */
  zipfileWrite32(a, ZIPFILE_SIGNATURE_LFH);
  zipfileWrite16(a, pCds->iVersionExtract);
  zipfileWrite16(a, pCds->flags);
  zipfileWrite16(a, pCds->iCompression);
  zipfileWrite16(a, pCds->mTime);
  zipfileWrite16(a, pCds->mDate);
  zipfileWrite32(a, pCds->crc32);
  zipfileWrite32(a, pCds->szCompressed);
  zipfileWrite32(a, pCds->szUncompressed);
  zipfileWrite16(a, (u16)pCds->nFile);
  zipfileWrite16(a, pCds->nExtra);
  assert( a==&aBuf[ZIPFILE_LFH_FIXED_SZ] );

  /* Add the file name */
  memcpy(a, pCds->zFile, (int)pCds->nFile);
  a += (int)pCds->nFile;

  /* The "extra" data */
  zipfileWrite16(a, ZIPFILE_EXTRA_TIMESTAMP);
  zipfileWrite16(a, 5);
  *a++ = 0x01;
  zipfileWrite32(a, pEntry->mUnixTime);

  return a-aBuf;
}