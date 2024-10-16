static void returnSingleText(
  Vdbe *v,                /* Prepared statement under construction */
  const char *zValue      /* Value to be returned */
){
  if( zValue ){
    sqlite3VdbeLoadString(v, 1, (const char*)zValue);
    sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
  }
}