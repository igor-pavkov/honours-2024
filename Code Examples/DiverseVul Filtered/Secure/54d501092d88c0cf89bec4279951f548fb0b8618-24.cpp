static int zipfileRegister(sqlite3 *db){
  static sqlite3_module zipfileModule = {
    1,                         /* iVersion */
    zipfileConnect,            /* xCreate */
    zipfileConnect,            /* xConnect */
    zipfileBestIndex,          /* xBestIndex */
    zipfileDisconnect,         /* xDisconnect */
    zipfileDisconnect,         /* xDestroy */
    zipfileOpen,               /* xOpen - open a cursor */
    zipfileClose,              /* xClose - close a cursor */
    zipfileFilter,             /* xFilter - configure scan constraints */
    zipfileNext,               /* xNext - advance a cursor */
    zipfileEof,                /* xEof - check for end of scan */
    zipfileColumn,             /* xColumn - read data */
    0,                         /* xRowid - read data */
    zipfileUpdate,             /* xUpdate */
    zipfileBegin,              /* xBegin */
    0,                         /* xSync */
    zipfileCommit,             /* xCommit */
    zipfileRollback,           /* xRollback */
    zipfileFindFunction,       /* xFindMethod */
    0,                         /* xRename */
  };

  int rc = sqlite3_create_module(db, "zipfile"  , &zipfileModule, 0);
  if( rc==SQLITE_OK ) rc = sqlite3_overload_function(db, "zipfile_cds", -1);
  if( rc==SQLITE_OK ){
    rc = sqlite3_create_function(db, "zipfile", -1, SQLITE_UTF8, 0, 0, 
        zipfileStep, zipfileFinal
    );
  }
  return rc;
}