static int zipfileAppendEOCD(ZipfileTab *pTab, ZipfileEOCD *p){
  int nBuf = zipfileSerializeEOCD(p, pTab->aBuffer);
  assert( nBuf==ZIPFILE_EOCD_FIXED_SZ );
  return zipfileAppendData(pTab, pTab->aBuffer, nBuf);
}