static u32 zipfileGetTime(sqlite3_value *pVal){
  if( pVal==0 || sqlite3_value_type(pVal)==SQLITE_NULL ){
    return zipfileTime();
  }
  return (u32)sqlite3_value_int64(pVal);
}