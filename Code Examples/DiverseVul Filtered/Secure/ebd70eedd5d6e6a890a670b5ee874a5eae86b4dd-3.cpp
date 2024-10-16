static int getAutoVacuum(const char *z){
  int i;
  if( 0==sqlite3StrICmp(z, "none") ) return BTREE_AUTOVACUUM_NONE;
  if( 0==sqlite3StrICmp(z, "full") ) return BTREE_AUTOVACUUM_FULL;
  if( 0==sqlite3StrICmp(z, "incremental") ) return BTREE_AUTOVACUUM_INCR;
  i = sqlite3Atoi(z);
  return (u8)((i>=0&&i<=2)?i:0);
}