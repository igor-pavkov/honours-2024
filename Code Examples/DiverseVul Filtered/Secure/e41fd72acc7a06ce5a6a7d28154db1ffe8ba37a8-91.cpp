static void rtreecheck(
  sqlite3_context *ctx, 
  int nArg, 
  sqlite3_value **apArg
){
  if( nArg!=1 && nArg!=2 ){
    sqlite3_result_error(ctx, 
        "wrong number of arguments to function rtreecheck()", -1
    );
  }else{
    int rc;
    char *zReport = 0;
    const char *zDb = (const char*)sqlite3_value_text(apArg[0]);
    const char *zTab;
    if( nArg==1 ){
      zTab = zDb;
      zDb = "main";
    }else{
      zTab = (const char*)sqlite3_value_text(apArg[1]);
    }
    rc = rtreeCheckTable(sqlite3_context_db_handle(ctx), zDb, zTab, &zReport);
    if( rc==SQLITE_OK ){
      sqlite3_result_text(ctx, zReport ? zReport : "ok", -1, SQLITE_TRANSIENT);
    }else{
      sqlite3_result_error_code(ctx, rc);
    }
    sqlite3_free(zReport);
  }
}