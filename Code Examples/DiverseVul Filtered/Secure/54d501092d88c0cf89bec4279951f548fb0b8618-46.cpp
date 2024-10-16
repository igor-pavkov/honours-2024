static int zipfileReadEOCD(
  ZipfileTab *pTab,               /* Return errors here */
  const u8 *aBlob,                /* Pointer to in-memory file image */
  int nBlob,                      /* Size of aBlob[] in bytes */
  FILE *pFile,                    /* Read from this file if aBlob==0 */
  ZipfileEOCD *pEOCD              /* Object to populate */
){
  u8 *aRead = pTab->aBuffer;      /* Temporary buffer */
  int nRead;                      /* Bytes to read from file */
  int rc = SQLITE_OK;

  if( aBlob==0 ){
    i64 iOff;                     /* Offset to read from */
    i64 szFile;                   /* Total size of file in bytes */
    fseek(pFile, 0, SEEK_END);
    szFile = (i64)ftell(pFile);
    if( szFile==0 ){
      memset(pEOCD, 0, sizeof(ZipfileEOCD));
      return SQLITE_OK;
    }
    nRead = (int)(MIN(szFile, ZIPFILE_BUFFER_SIZE));
    iOff = szFile - nRead;
    rc = zipfileReadData(pFile, aRead, nRead, iOff, &pTab->base.zErrMsg);
  }else{
    nRead = (int)(MIN(nBlob, ZIPFILE_BUFFER_SIZE));
    aRead = (u8*)&aBlob[nBlob-nRead];
  }

  if( rc==SQLITE_OK ){
    int i;

    /* Scan backwards looking for the signature bytes */
    for(i=nRead-20; i>=0; i--){
      if( aRead[i]==0x50 && aRead[i+1]==0x4b 
       && aRead[i+2]==0x05 && aRead[i+3]==0x06 
      ){
        break;
      }
    }
    if( i<0 ){
      pTab->base.zErrMsg = sqlite3_mprintf(
          "cannot find end of central directory record"
      );
      return SQLITE_ERROR;
    }

    aRead += i+4;
    pEOCD->iDisk = zipfileRead16(aRead);
    pEOCD->iFirstDisk = zipfileRead16(aRead);
    pEOCD->nEntry = zipfileRead16(aRead);
    pEOCD->nEntryTotal = zipfileRead16(aRead);
    pEOCD->nSize = zipfileRead32(aRead);
    pEOCD->iOffset = zipfileRead32(aRead);
  }

  return rc;
}