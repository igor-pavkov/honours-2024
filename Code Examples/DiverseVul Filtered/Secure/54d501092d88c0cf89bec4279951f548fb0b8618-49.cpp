static int zipfileFindFunction(
  sqlite3_vtab *pVtab,            /* Virtual table handle */
  int nArg,                       /* Number of SQL function arguments */
  const char *zName,              /* Name of SQL function */
  void (**pxFunc)(sqlite3_context*,int,sqlite3_value**), /* OUT: Result */
  void **ppArg                    /* OUT: User data for *pxFunc */
){
  if( sqlite3_stricmp("zipfile_cds", zName)==0 ){
    *pxFunc = zipfileFunctionCds;
    *ppArg = (void*)pVtab;
    return 1;
  }
  return 0;
}