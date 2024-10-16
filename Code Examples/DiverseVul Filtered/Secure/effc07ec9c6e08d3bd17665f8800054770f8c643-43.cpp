int sqlite3WhereIsOrdered(WhereInfo *pWInfo){
  return pWInfo->nOBSat;
}