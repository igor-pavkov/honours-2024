static int zipfileDisconnect(sqlite3_vtab *pVtab){
  zipfileCleanupTransaction((ZipfileTab*)pVtab);
  sqlite3_free(pVtab);
  return SQLITE_OK;
}