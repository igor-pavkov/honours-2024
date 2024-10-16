void sqlite3VtabUsesAllSchemas(sqlite3_index_info *pIdxInfo){
  HiddenIndexInfo *pHidden = (HiddenIndexInfo*)&pIdxInfo[1];
  Parse *pParse = pHidden->pParse;
  int nDb = pParse->db->nDb;
  int i;
  for(i=0; i<nDb; i++){
    sqlite3CodeVerifySchema(pParse, i);
  }
  if( pParse->writeMask ){
    for(i=0; i<nDb; i++){
      sqlite3BeginWriteOperation(pParse, 0, i);
    }
  }
}