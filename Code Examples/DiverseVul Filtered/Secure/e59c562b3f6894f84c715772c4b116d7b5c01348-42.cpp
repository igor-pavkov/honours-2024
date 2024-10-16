static int withExpand(
  Walker *pWalker, 
  struct SrcList_item *pFrom
){
  Parse *pParse = pWalker->pParse;
  sqlite3 *db = pParse->db;
  struct Cte *pCte;               /* Matched CTE (or NULL if no match) */
  With *pWith;                    /* WITH clause that pCte belongs to */

  assert( pFrom->pTab==0 );
  if( pParse->nErr ){
    return SQLITE_ERROR;
  }

  pCte = searchWith(pParse->pWith, pFrom, &pWith);
  if( pCte ){
    Table *pTab;
    ExprList *pEList;
    Select *pSel;
    Select *pLeft;                /* Left-most SELECT statement */
    int bMayRecursive;            /* True if compound joined by UNION [ALL] */
    With *pSavedWith;             /* Initial value of pParse->pWith */

    /* If pCte->zCteErr is non-NULL at this point, then this is an illegal
    ** recursive reference to CTE pCte. Leave an error in pParse and return
    ** early. If pCte->zCteErr is NULL, then this is not a recursive reference.
    ** In this case, proceed.  */
    if( pCte->zCteErr ){
      sqlite3ErrorMsg(pParse, pCte->zCteErr, pCte->zName);
      return SQLITE_ERROR;
    }
    if( cannotBeFunction(pParse, pFrom) ) return SQLITE_ERROR;

    assert( pFrom->pTab==0 );
    pFrom->pTab = pTab = sqlite3DbMallocZero(db, sizeof(Table));
    if( pTab==0 ) return WRC_Abort;
    pTab->nTabRef = 1;
    pTab->zName = sqlite3DbStrDup(db, pCte->zName);
    pTab->iPKey = -1;
    pTab->nRowLogEst = 200; assert( 200==sqlite3LogEst(1048576) );
    pTab->tabFlags |= TF_Ephemeral | TF_NoVisibleRowid;
    pFrom->pSelect = sqlite3SelectDup(db, pCte->pSelect, 0);
    if( db->mallocFailed ) return SQLITE_NOMEM_BKPT;
    assert( pFrom->pSelect );

    /* Check if this is a recursive CTE. */
    pSel = pFrom->pSelect;
    bMayRecursive = ( pSel->op==TK_ALL || pSel->op==TK_UNION );
    if( bMayRecursive ){
      int i;
      SrcList *pSrc = pFrom->pSelect->pSrc;
      for(i=0; i<pSrc->nSrc; i++){
        struct SrcList_item *pItem = &pSrc->a[i];
        if( pItem->zDatabase==0 
         && pItem->zName!=0 
         && 0==sqlite3StrICmp(pItem->zName, pCte->zName)
          ){
          pItem->pTab = pTab;
          pItem->fg.isRecursive = 1;
          pTab->nTabRef++;
          pSel->selFlags |= SF_Recursive;
        }
      }
    }

    /* Only one recursive reference is permitted. */ 
    if( pTab->nTabRef>2 ){
      sqlite3ErrorMsg(
          pParse, "multiple references to recursive table: %s", pCte->zName
      );
      return SQLITE_ERROR;
    }
    assert( pTab->nTabRef==1 || 
            ((pSel->selFlags&SF_Recursive) && pTab->nTabRef==2 ));

    pCte->zCteErr = "circular reference: %s";
    pSavedWith = pParse->pWith;
    pParse->pWith = pWith;
    if( bMayRecursive ){
      Select *pPrior = pSel->pPrior;
      assert( pPrior->pWith==0 );
      pPrior->pWith = pSel->pWith;
      sqlite3WalkSelect(pWalker, pPrior);
      pPrior->pWith = 0;
    }else{
      sqlite3WalkSelect(pWalker, pSel);
    }
    pParse->pWith = pWith;

    for(pLeft=pSel; pLeft->pPrior; pLeft=pLeft->pPrior);
    pEList = pLeft->pEList;
    if( pCte->pCols ){
      if( pEList && pEList->nExpr!=pCte->pCols->nExpr ){
        sqlite3ErrorMsg(pParse, "table %s has %d values for %d columns",
            pCte->zName, pEList->nExpr, pCte->pCols->nExpr
        );
        pParse->pWith = pSavedWith;
        return SQLITE_ERROR;
      }
      pEList = pCte->pCols;
    }

    sqlite3ColumnsFromExprList(pParse, pEList, &pTab->nCol, &pTab->aCol);
    if( bMayRecursive ){
      if( pSel->selFlags & SF_Recursive ){
        pCte->zCteErr = "multiple recursive references: %s";
      }else{
        pCte->zCteErr = "recursive reference in a subquery: %s";
      }
      sqlite3WalkSelect(pWalker, pSel);
    }
    pCte->zCteErr = 0;
    pParse->pWith = pSavedWith;
  }

  return SQLITE_OK;
}