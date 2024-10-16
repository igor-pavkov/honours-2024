static void zipfileCleanupTransaction(ZipfileTab *pTab){
  ZipfileEntry *pEntry;
  ZipfileEntry *pNext;

  if( pTab->pWriteFd ){
    fclose(pTab->pWriteFd);
    pTab->pWriteFd = 0;
  }
  for(pEntry=pTab->pFirstEntry; pEntry; pEntry=pNext){
    pNext = pEntry->pNext;
    zipfileEntryFree(pEntry);
  }
  pTab->pFirstEntry = 0;
  pTab->pLastEntry = 0;
  pTab->szCurrent = 0;
  pTab->szOrig = 0;
}