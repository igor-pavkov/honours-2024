u8 sqlite3GetBoolean(const char *z, u8 dflt){
  return getSafetyLevel(z,1,dflt)!=0;
}