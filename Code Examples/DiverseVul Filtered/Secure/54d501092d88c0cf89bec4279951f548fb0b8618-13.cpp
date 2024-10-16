static int zipfileSerializeEOCD(ZipfileEOCD *p, u8 *aBuf){
  u8 *a = aBuf;
  zipfileWrite32(a, ZIPFILE_SIGNATURE_EOCD);
  zipfileWrite16(a, p->iDisk);
  zipfileWrite16(a, p->iFirstDisk);
  zipfileWrite16(a, p->nEntry);
  zipfileWrite16(a, p->nEntryTotal);
  zipfileWrite32(a, p->nSize);
  zipfileWrite32(a, p->iOffset);
  zipfileWrite16(a, 0);        /* Size of trailing comment in bytes*/

  return a-aBuf;
}