Module *sqlite3PragmaVtabRegister(sqlite3 *db, const char *zName){
  const PragmaName *pName;
  assert( sqlite3_strnicmp(zName, "pragma_", 7)==0 );
  pName = pragmaLocate(zName+7);
  if( pName==0 ) return 0;
  if( (pName->mPragFlg & (PragFlg_Result0|PragFlg_Result1))==0 ) return 0;
  assert( sqlite3HashFind(&db->aModule, zName)==0 );
  return sqlite3VtabCreateModule(db, zName, &pragmaVtabModule, (void*)pName, 0);
}