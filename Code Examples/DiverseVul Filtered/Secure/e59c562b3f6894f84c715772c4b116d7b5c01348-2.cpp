static void resetAccumulator(Parse *pParse, AggInfo *pAggInfo){
  Vdbe *v = pParse->pVdbe;
  int i;
  struct AggInfo_func *pFunc;
  int nReg = pAggInfo->nFunc + pAggInfo->nColumn;
  if( nReg==0 ) return;
#ifdef SQLITE_DEBUG
  /* Verify that all AggInfo registers are within the range specified by
  ** AggInfo.mnReg..AggInfo.mxReg */
  assert( nReg==pAggInfo->mxReg-pAggInfo->mnReg+1 );
  for(i=0; i<pAggInfo->nColumn; i++){
    assert( pAggInfo->aCol[i].iMem>=pAggInfo->mnReg
         && pAggInfo->aCol[i].iMem<=pAggInfo->mxReg );
  }
  for(i=0; i<pAggInfo->nFunc; i++){
    assert( pAggInfo->aFunc[i].iMem>=pAggInfo->mnReg
         && pAggInfo->aFunc[i].iMem<=pAggInfo->mxReg );
  }
#endif
  sqlite3VdbeAddOp3(v, OP_Null, 0, pAggInfo->mnReg, pAggInfo->mxReg);
  for(pFunc=pAggInfo->aFunc, i=0; i<pAggInfo->nFunc; i++, pFunc++){
    if( pFunc->iDistinct>=0 ){
      Expr *pE = pFunc->pExpr;
      assert( !ExprHasProperty(pE, EP_xIsSelect) );
      if( pE->x.pList==0 || pE->x.pList->nExpr!=1 ){
        sqlite3ErrorMsg(pParse, "DISTINCT aggregates must have exactly one "
           "argument");
        pFunc->iDistinct = -1;
      }else{
        KeyInfo *pKeyInfo = sqlite3KeyInfoFromExprList(pParse, pE->x.pList,0,0);
        sqlite3VdbeAddOp4(v, OP_OpenEphemeral, pFunc->iDistinct, 0, 0,
                          (char*)pKeyInfo, P4_KEYINFO);
      }
    }
  }
}