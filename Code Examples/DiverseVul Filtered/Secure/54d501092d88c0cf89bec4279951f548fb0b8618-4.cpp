static void zipfileRemoveEntryFromList(ZipfileTab *pTab, ZipfileEntry *pOld){
  if( pOld ){
    ZipfileEntry **pp;
    for(pp=&pTab->pFirstEntry; (*pp)!=pOld; pp=&((*pp)->pNext));
    *pp = (*pp)->pNext;
    zipfileEntryFree(pOld);
  }
}