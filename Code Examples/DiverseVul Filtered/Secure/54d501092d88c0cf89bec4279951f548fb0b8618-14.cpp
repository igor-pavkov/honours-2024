static ZipfileCsr *zipfileFindCursor(ZipfileTab *pTab, i64 iId){
  ZipfileCsr *pCsr;
  for(pCsr=pTab->pCsrList; pCsr; pCsr=pCsr->pCsrNext){
    if( iId==pCsr->iId ) break;
  }
  return pCsr;
}