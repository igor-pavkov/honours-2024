static void returnSingleInt(Vdbe *v, i64 value){
  sqlite3VdbeAddOp4Dup8(v, OP_Int64, 0, 1, 0, (const u8*)&value, P4_INT64);
  sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
}