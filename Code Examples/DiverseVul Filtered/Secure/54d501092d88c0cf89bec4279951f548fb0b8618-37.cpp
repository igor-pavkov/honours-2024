static int zipfileConnect(
  sqlite3 *db,
  void *pAux,
  int argc, const char *const*argv,
  sqlite3_vtab **ppVtab,
  char **pzErr
){
  int nByte = sizeof(ZipfileTab) + ZIPFILE_BUFFER_SIZE;
  int nFile = 0;
  const char *zFile = 0;
  ZipfileTab *pNew = 0;
  int rc;

  /* If the table name is not "zipfile", require that the argument be
  ** specified. This stops zipfile tables from being created as:
  **
  **   CREATE VIRTUAL TABLE zzz USING zipfile();
  **
  ** It does not prevent:
  **
  **   CREATE VIRTUAL TABLE zipfile USING zipfile();
  */
  assert( 0==sqlite3_stricmp(argv[0], "zipfile") );
  if( (0!=sqlite3_stricmp(argv[2], "zipfile") && argc<4) || argc>4 ){
    *pzErr = sqlite3_mprintf("zipfile constructor requires one argument");
    return SQLITE_ERROR;
  }

  if( argc>3 ){
    zFile = argv[3];
    nFile = (int)strlen(zFile)+1;
  }

  rc = sqlite3_declare_vtab(db, ZIPFILE_SCHEMA);
  if( rc==SQLITE_OK ){
    pNew = (ZipfileTab*)sqlite3_malloc64((sqlite3_int64)nByte+nFile);
    if( pNew==0 ) return SQLITE_NOMEM;
    memset(pNew, 0, nByte+nFile);
    pNew->db = db;
    pNew->aBuffer = (u8*)&pNew[1];
    if( zFile ){
      pNew->zFile = (char*)&pNew->aBuffer[ZIPFILE_BUFFER_SIZE];
      memcpy(pNew->zFile, zFile, nFile);
      zipfileDequote(pNew->zFile);
    }
  }
  *ppVtab = (sqlite3_vtab*)pNew;
  return rc;
}