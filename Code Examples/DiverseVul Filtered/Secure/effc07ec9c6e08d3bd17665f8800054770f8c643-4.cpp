int sqlite3WhereUsesDeferredSeek(WhereInfo *pWInfo){
  return pWInfo->bDeferredSeek;
}