static void zipfileResetCursor(ZipfileCsr *pCsr){
  ZipfileEntry *p;
  ZipfileEntry *pNext;

  pCsr->bEof = 0;
  if( pCsr->pFile ){
    fclose(pCsr->pFile);
    pCsr->pFile = 0;
    zipfileEntryFree(pCsr->pCurrent);
    pCsr->pCurrent = 0;
  }

  for(p=pCsr->pFreeEntry; p; p=pNext){
    pNext = p->pNext;
    zipfileEntryFree(p);
  }
}