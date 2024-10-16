static void zipfileAddEntry(
  ZipfileTab *pTab, 
  ZipfileEntry *pBefore, 
  ZipfileEntry *pNew
){
  assert( (pTab->pFirstEntry==0)==(pTab->pLastEntry==0) );
  assert( pNew->pNext==0 );
  if( pBefore==0 ){
    if( pTab->pFirstEntry==0 ){
      pTab->pFirstEntry = pTab->pLastEntry = pNew;
    }else{
      assert( pTab->pLastEntry->pNext==0 );
      pTab->pLastEntry->pNext = pNew;
      pTab->pLastEntry = pNew;
    }
  }else{
    ZipfileEntry **pp;
    for(pp=&pTab->pFirstEntry; *pp!=pBefore; pp=&((*pp)->pNext));
    pNew->pNext = pBefore;
    *pp = pNew;
  }
}