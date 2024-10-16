int sqlite3WhereIsDistinct(WhereInfo *pWInfo){
  return pWInfo->eDistinct;
}