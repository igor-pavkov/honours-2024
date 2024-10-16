static WhereTerm *whereScanNext(WhereScan *pScan){
  int iCur;            /* The cursor on the LHS of the term */
  i16 iColumn;         /* The column on the LHS of the term.  -1 for IPK */
  Expr *pX;            /* An expression being tested */
  WhereClause *pWC;    /* Shorthand for pScan->pWC */
  WhereTerm *pTerm;    /* The term being tested */
  int k = pScan->k;    /* Where to start scanning */

  assert( pScan->iEquiv<=pScan->nEquiv );
  pWC = pScan->pWC;
  while(1){
    iColumn = pScan->aiColumn[pScan->iEquiv-1];
    iCur = pScan->aiCur[pScan->iEquiv-1];
    assert( pWC!=0 );
    assert( iCur>=0 );
    do{
      for(pTerm=pWC->a+k; k<pWC->nTerm; k++, pTerm++){
        assert( (pTerm->eOperator & (WO_OR|WO_AND))==0 || pTerm->leftCursor<0 );
        if( pTerm->leftCursor==iCur
         && pTerm->u.x.leftColumn==iColumn
         && (iColumn!=XN_EXPR
             || sqlite3ExprCompareSkip(pTerm->pExpr->pLeft,
                                       pScan->pIdxExpr,iCur)==0)
         && (pScan->iEquiv<=1 || !ExprHasProperty(pTerm->pExpr, EP_OuterON))
        ){
          if( (pTerm->eOperator & WO_EQUIV)!=0
           && pScan->nEquiv<ArraySize(pScan->aiCur)
           && (pX = whereRightSubexprIsColumn(pTerm->pExpr))!=0
          ){
            int j;
            for(j=0; j<pScan->nEquiv; j++){
              if( pScan->aiCur[j]==pX->iTable
               && pScan->aiColumn[j]==pX->iColumn ){
                  break;
              }
            }
            if( j==pScan->nEquiv ){
              pScan->aiCur[j] = pX->iTable;
              pScan->aiColumn[j] = pX->iColumn;
              pScan->nEquiv++;
            }
          }
          if( (pTerm->eOperator & pScan->opMask)!=0 ){
            /* Verify the affinity and collating sequence match */
            if( pScan->zCollName && (pTerm->eOperator & WO_ISNULL)==0 ){
              CollSeq *pColl;
              Parse *pParse = pWC->pWInfo->pParse;
              pX = pTerm->pExpr;
              if( !sqlite3IndexAffinityOk(pX, pScan->idxaff) ){
                continue;
              }
              assert(pX->pLeft);
              pColl = sqlite3ExprCompareCollSeq(pParse, pX);
              if( pColl==0 ) pColl = pParse->db->pDfltColl;
              if( sqlite3StrICmp(pColl->zName, pScan->zCollName) ){
                continue;
              }
            }
            if( (pTerm->eOperator & (WO_EQ|WO_IS))!=0
             && (pX = pTerm->pExpr->pRight, ALWAYS(pX!=0))
             && pX->op==TK_COLUMN
             && pX->iTable==pScan->aiCur[0]
             && pX->iColumn==pScan->aiColumn[0]
            ){
              testcase( pTerm->eOperator & WO_IS );
              continue;
            }
            pScan->pWC = pWC;
            pScan->k = k+1;
#ifdef WHERETRACE_ENABLED
            if( sqlite3WhereTrace & 0x20000 ){
              int ii;
              sqlite3DebugPrintf("SCAN-TERM %p: nEquiv=%d",
                 pTerm, pScan->nEquiv);
              for(ii=0; ii<pScan->nEquiv; ii++){
                sqlite3DebugPrintf(" {%d:%d}",
                   pScan->aiCur[ii], pScan->aiColumn[ii]);
              }
              sqlite3DebugPrintf("\n");
            }
#endif
            return pTerm;
          }
        }
      }
      pWC = pWC->pOuter;
      k = 0;
    }while( pWC!=0 );
    if( pScan->iEquiv>=pScan->nEquiv ) break;
    pWC = pScan->pOrigWC;
    k = 0;
    pScan->iEquiv++;
  }
  return 0;
}