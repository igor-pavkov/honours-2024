static int zipfileNext(sqlite3_vtab_cursor *cur){
  ZipfileCsr *pCsr = (ZipfileCsr*)cur;
  int rc = SQLITE_OK;

  if( pCsr->pFile ){
    i64 iEof = pCsr->eocd.iOffset + pCsr->eocd.nSize;
    zipfileEntryFree(pCsr->pCurrent);
    pCsr->pCurrent = 0;
    if( pCsr->iNextOff>=iEof ){
      pCsr->bEof = 1;
    }else{
      ZipfileEntry *p = 0;
      ZipfileTab *pTab = (ZipfileTab*)(cur->pVtab);
      rc = zipfileGetEntry(pTab, 0, 0, pCsr->pFile, pCsr->iNextOff, &p);
      if( rc==SQLITE_OK ){
        pCsr->iNextOff += ZIPFILE_CDS_FIXED_SZ;
        pCsr->iNextOff += (int)p->cds.nExtra + p->cds.nFile + p->cds.nComment;
      }
      pCsr->pCurrent = p;
    }
  }else{
    if( !pCsr->bNoop ){
      pCsr->pCurrent = pCsr->pCurrent->pNext;
    }
    if( pCsr->pCurrent==0 ){
      pCsr->bEof = 1;
    }
  }

  pCsr->bNoop = 0;
  return rc;
}