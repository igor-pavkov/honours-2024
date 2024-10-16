static int zipfileRollback(sqlite3_vtab *pVtab){
  return zipfileCommit(pVtab);
}