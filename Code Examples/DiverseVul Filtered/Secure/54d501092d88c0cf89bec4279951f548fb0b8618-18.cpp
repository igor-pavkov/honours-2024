static int zipfileGetEntry(
  ZipfileTab *pTab,               /* Store any error message here */
  const u8 *aBlob,                /* Pointer to in-memory file image */
  int nBlob,                      /* Size of aBlob[] in bytes */
  FILE *pFile,                    /* If aBlob==0, read from this file */
  i64 iOff,                       /* Offset of CDS record */
  ZipfileEntry **ppEntry          /* OUT: Pointer to new object */
){
  u8 *aRead;
  char **pzErr = &pTab->base.zErrMsg;
  int rc = SQLITE_OK;

  if( aBlob==0 ){
    aRead = pTab->aBuffer;
    rc = zipfileReadData(pFile, aRead, ZIPFILE_CDS_FIXED_SZ, iOff, pzErr);
  }else{
    aRead = (u8*)&aBlob[iOff];
  }

  if( rc==SQLITE_OK ){
    sqlite3_int64 nAlloc;
    ZipfileEntry *pNew;

    int nFile = zipfileGetU16(&aRead[ZIPFILE_CDS_NFILE_OFF]);
    int nExtra = zipfileGetU16(&aRead[ZIPFILE_CDS_NFILE_OFF+2]);
    nExtra += zipfileGetU16(&aRead[ZIPFILE_CDS_NFILE_OFF+4]);

    nAlloc = sizeof(ZipfileEntry) + nExtra;
    if( aBlob ){
      nAlloc += zipfileGetU32(&aRead[ZIPFILE_CDS_SZCOMPRESSED_OFF]);
    }

    pNew = (ZipfileEntry*)sqlite3_malloc64(nAlloc);
    if( pNew==0 ){
      rc = SQLITE_NOMEM;
    }else{
      memset(pNew, 0, sizeof(ZipfileEntry));
      rc = zipfileReadCDS(aRead, &pNew->cds);
      if( rc!=SQLITE_OK ){
        *pzErr = sqlite3_mprintf("failed to read CDS at offset %lld", iOff);
      }else if( aBlob==0 ){
        rc = zipfileReadData(
            pFile, aRead, nExtra+nFile, iOff+ZIPFILE_CDS_FIXED_SZ, pzErr
        );
      }else{
        aRead = (u8*)&aBlob[iOff + ZIPFILE_CDS_FIXED_SZ];
      }
    }

    if( rc==SQLITE_OK ){
      u32 *pt = &pNew->mUnixTime;
      pNew->cds.zFile = sqlite3_mprintf("%.*s", nFile, aRead); 
      pNew->aExtra = (u8*)&pNew[1];
      memcpy(pNew->aExtra, &aRead[nFile], nExtra);
      if( pNew->cds.zFile==0 ){
        rc = SQLITE_NOMEM;
      }else if( 0==zipfileScanExtra(&aRead[nFile], pNew->cds.nExtra, pt) ){
        pNew->mUnixTime = zipfileMtime(&pNew->cds);
      }
    }

    if( rc==SQLITE_OK ){
      static const int szFix = ZIPFILE_LFH_FIXED_SZ;
      ZipfileLFH lfh;
      if( pFile ){
        rc = zipfileReadData(pFile, aRead, szFix, pNew->cds.iOffset, pzErr);
      }else{
        aRead = (u8*)&aBlob[pNew->cds.iOffset];
      }

      rc = zipfileReadLFH(aRead, &lfh);
      if( rc==SQLITE_OK ){
        pNew->iDataOff =  pNew->cds.iOffset + ZIPFILE_LFH_FIXED_SZ;
        pNew->iDataOff += lfh.nFile + lfh.nExtra;
        if( aBlob && pNew->cds.szCompressed ){
          pNew->aData = &pNew->aExtra[nExtra];
          memcpy(pNew->aData, &aBlob[pNew->iDataOff], pNew->cds.szCompressed);
        }
      }else{
        *pzErr = sqlite3_mprintf("failed to read LFH at offset %d", 
            (int)pNew->cds.iOffset
        );
      }
    }

    if( rc!=SQLITE_OK ){
      zipfileEntryFree(pNew);
    }else{
      *ppEntry = pNew;
    }
  }

  return rc;
}