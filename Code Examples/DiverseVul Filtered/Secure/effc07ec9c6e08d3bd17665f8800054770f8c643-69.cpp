  static void sqlite3WhereOpcodeRewriteTrace(
    sqlite3 *db,
    int pc,
    VdbeOp *pOp
  ){
    if( (db->flags & SQLITE_VdbeAddopTrace)==0 ) return;
    sqlite3VdbePrintOp(0, pc, pOp);
  }