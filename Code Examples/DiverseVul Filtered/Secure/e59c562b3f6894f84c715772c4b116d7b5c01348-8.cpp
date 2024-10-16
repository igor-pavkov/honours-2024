void sqlite3SelectDelete(sqlite3 *db, Select *p){
  if( OK_IF_ALWAYS_TRUE(p) ) clearSelect(db, p, 1);
}