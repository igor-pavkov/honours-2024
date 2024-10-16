static void geomCallback(sqlite3_context *ctx, int nArg, sqlite3_value **aArg){
  RtreeGeomCallback *pGeomCtx = (RtreeGeomCallback *)sqlite3_user_data(ctx);
  RtreeMatchArg *pBlob;
  sqlite3_int64 nBlob;
  int memErr = 0;

  nBlob = sizeof(RtreeMatchArg) + (nArg-1)*sizeof(RtreeDValue)
           + nArg*sizeof(sqlite3_value*);
  pBlob = (RtreeMatchArg *)sqlite3_malloc64(nBlob);
  if( !pBlob ){
    sqlite3_result_error_nomem(ctx);
  }else{
    int i;
    pBlob->iSize = nBlob;
    pBlob->cb = pGeomCtx[0];
    pBlob->apSqlParam = (sqlite3_value**)&pBlob->aParam[nArg];
    pBlob->nParam = nArg;
    for(i=0; i<nArg; i++){
      pBlob->apSqlParam[i] = sqlite3_value_dup(aArg[i]);
      if( pBlob->apSqlParam[i]==0 ) memErr = 1;
#ifdef SQLITE_RTREE_INT_ONLY
      pBlob->aParam[i] = sqlite3_value_int64(aArg[i]);
#else
      pBlob->aParam[i] = sqlite3_value_double(aArg[i]);
#endif
    }
    if( memErr ){
      sqlite3_result_error_nomem(ctx);
      rtreeMatchArgFree(pBlob);
    }else{
      sqlite3_result_pointer(ctx, pBlob, "RtreeMatchArg", rtreeMatchArgFree);
    }
  }
}