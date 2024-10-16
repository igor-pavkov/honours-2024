static int rtreeShadowName(const char *zName){
  static const char *azName[] = {
    "node", "parent", "rowid"
  };
  unsigned int i;
  for(i=0; i<sizeof(azName)/sizeof(azName[0]); i++){
    if( sqlite3_stricmp(zName, azName[i])==0 ) return 1;
  }
  return 0;
}