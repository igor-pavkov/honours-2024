static sqlite3_index_info *allocateIndexInfo(
  WhereInfo *pWInfo,              /* The WHERE clause */
  WhereClause *pWC,               /* The WHERE clause being analyzed */
  Bitmask mUnusable,              /* Ignore terms with these prereqs */
  SrcItem *pSrc,                  /* The FROM clause term that is the vtab */
  u16 *pmNoOmit                   /* Mask of terms not to omit */
){
  int i, j;
  int nTerm;
  Parse *pParse = pWInfo->pParse;
  struct sqlite3_index_constraint *pIdxCons;
  struct sqlite3_index_orderby *pIdxOrderBy;
  struct sqlite3_index_constraint_usage *pUsage;
  struct HiddenIndexInfo *pHidden;
  WhereTerm *pTerm;
  int nOrderBy;
  sqlite3_index_info *pIdxInfo;
  u16 mNoOmit = 0;
  const Table *pTab;
  int eDistinct = 0;
  ExprList *pOrderBy = pWInfo->pOrderBy;
 
  assert( pSrc!=0 );
  pTab = pSrc->pTab;
  assert( pTab!=0 );
  assert( IsVirtual(pTab) );

  /* Find all WHERE clause constraints referring to this virtual table.
  ** Mark each term with the TERM_OK flag.  Set nTerm to the number of
  ** terms found.
  */
  for(i=nTerm=0, pTerm=pWC->a; i<pWC->nTerm; i++, pTerm++){
    pTerm->wtFlags &= ~TERM_OK;
    if( pTerm->leftCursor != pSrc->iCursor ) continue;
    if( pTerm->prereqRight & mUnusable ) continue;
    assert( IsPowerOfTwo(pTerm->eOperator & ~WO_EQUIV) );
    testcase( pTerm->eOperator & WO_IN );
    testcase( pTerm->eOperator & WO_ISNULL );
    testcase( pTerm->eOperator & WO_IS );
    testcase( pTerm->eOperator & WO_ALL );
    if( (pTerm->eOperator & ~(WO_EQUIV))==0 ) continue;
    if( pTerm->wtFlags & TERM_VNULL ) continue;

    assert( (pTerm->eOperator & (WO_OR|WO_AND))==0 );
    assert( pTerm->u.x.leftColumn>=XN_ROWID );
    assert( pTerm->u.x.leftColumn<pTab->nCol );

    /* tag-20191211-002: WHERE-clause constraints are not useful to the
    ** right-hand table of a LEFT JOIN nor to the either table of a
    ** RIGHT JOIN.  See tag-20191211-001 for the
    ** equivalent restriction for ordinary tables. */
    if( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))!=0 ){
      testcase( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))==JT_LEFT );
      testcase( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))==JT_RIGHT );
      testcase( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))==JT_LTORJ );
      testcase( ExprHasProperty(pTerm->pExpr, EP_OuterON) );
      testcase( ExprHasProperty(pTerm->pExpr, EP_InnerON) );
      if( !ExprHasProperty(pTerm->pExpr, EP_OuterON|EP_InnerON)
       || pTerm->pExpr->w.iJoin != pSrc->iCursor
      ){
        continue;
      }
    }
    nTerm++;
    pTerm->wtFlags |= TERM_OK;
  }

  /* If the ORDER BY clause contains only columns in the current 
  ** virtual table then allocate space for the aOrderBy part of
  ** the sqlite3_index_info structure.
  */
  nOrderBy = 0;
  if( pOrderBy ){
    int n = pOrderBy->nExpr;
    for(i=0; i<n; i++){
      Expr *pExpr = pOrderBy->a[i].pExpr;
      Expr *pE2;

      /* Skip over constant terms in the ORDER BY clause */
      if( sqlite3ExprIsConstant(pExpr) ){
        continue;
      }

      /* Virtual tables are unable to deal with NULLS FIRST */
      if( pOrderBy->a[i].fg.sortFlags & KEYINFO_ORDER_BIGNULL ) break;

      /* First case - a direct column references without a COLLATE operator */
      if( pExpr->op==TK_COLUMN && pExpr->iTable==pSrc->iCursor ){
        assert( pExpr->iColumn>=XN_ROWID && pExpr->iColumn<pTab->nCol );
        continue;
      }

      /* 2nd case - a column reference with a COLLATE operator.  Only match
      ** of the COLLATE operator matches the collation of the column. */
      if( pExpr->op==TK_COLLATE
       && (pE2 = pExpr->pLeft)->op==TK_COLUMN
       && pE2->iTable==pSrc->iCursor
      ){
        const char *zColl;  /* The collating sequence name */
        assert( !ExprHasProperty(pExpr, EP_IntValue) );
        assert( pExpr->u.zToken!=0 );
        assert( pE2->iColumn>=XN_ROWID && pE2->iColumn<pTab->nCol );
        pExpr->iColumn = pE2->iColumn;
        if( pE2->iColumn<0 ) continue;  /* Collseq does not matter for rowid */
        zColl = sqlite3ColumnColl(&pTab->aCol[pE2->iColumn]);
        if( zColl==0 ) zColl = sqlite3StrBINARY;
        if( sqlite3_stricmp(pExpr->u.zToken, zColl)==0 ) continue;
      }

      /* No matches cause a break out of the loop */
      break;
    }
    if( i==n ){
      nOrderBy = n;
      if( (pWInfo->wctrlFlags & WHERE_DISTINCTBY) ){
        eDistinct = 2 + ((pWInfo->wctrlFlags & WHERE_SORTBYGROUP)!=0);
      }else if( pWInfo->wctrlFlags & WHERE_GROUPBY ){
        eDistinct = 1;
      }
    }
  }

  /* Allocate the sqlite3_index_info structure
  */
  pIdxInfo = sqlite3DbMallocZero(pParse->db, sizeof(*pIdxInfo)
                           + (sizeof(*pIdxCons) + sizeof(*pUsage))*nTerm
                           + sizeof(*pIdxOrderBy)*nOrderBy + sizeof(*pHidden)
                           + sizeof(sqlite3_value*)*nTerm );
  if( pIdxInfo==0 ){
    sqlite3ErrorMsg(pParse, "out of memory");
    return 0;
  }
  pHidden = (struct HiddenIndexInfo*)&pIdxInfo[1];
  pIdxCons = (struct sqlite3_index_constraint*)&pHidden->aRhs[nTerm];
  pIdxOrderBy = (struct sqlite3_index_orderby*)&pIdxCons[nTerm];
  pUsage = (struct sqlite3_index_constraint_usage*)&pIdxOrderBy[nOrderBy];
  pIdxInfo->aConstraint = pIdxCons;
  pIdxInfo->aOrderBy = pIdxOrderBy;
  pIdxInfo->aConstraintUsage = pUsage;
  pHidden->pWC = pWC;
  pHidden->pParse = pParse;
  pHidden->eDistinct = eDistinct;
  pHidden->mIn = 0;
  for(i=j=0, pTerm=pWC->a; i<pWC->nTerm; i++, pTerm++){
    u16 op;
    if( (pTerm->wtFlags & TERM_OK)==0 ) continue;
    pIdxCons[j].iColumn = pTerm->u.x.leftColumn;
    pIdxCons[j].iTermOffset = i;
    op = pTerm->eOperator & WO_ALL;
    if( op==WO_IN ){
      if( (pTerm->wtFlags & TERM_SLICE)==0 ){
        pHidden->mIn |= SMASKBIT32(j);
      }
      op = WO_EQ;
    }
    if( op==WO_AUX ){
      pIdxCons[j].op = pTerm->eMatchOp;
    }else if( op & (WO_ISNULL|WO_IS) ){
      if( op==WO_ISNULL ){
        pIdxCons[j].op = SQLITE_INDEX_CONSTRAINT_ISNULL;
      }else{
        pIdxCons[j].op = SQLITE_INDEX_CONSTRAINT_IS;
      }
    }else{
      pIdxCons[j].op = (u8)op;
      /* The direct assignment in the previous line is possible only because
      ** the WO_ and SQLITE_INDEX_CONSTRAINT_ codes are identical.  The
      ** following asserts verify this fact. */
      assert( WO_EQ==SQLITE_INDEX_CONSTRAINT_EQ );
      assert( WO_LT==SQLITE_INDEX_CONSTRAINT_LT );
      assert( WO_LE==SQLITE_INDEX_CONSTRAINT_LE );
      assert( WO_GT==SQLITE_INDEX_CONSTRAINT_GT );
      assert( WO_GE==SQLITE_INDEX_CONSTRAINT_GE );
      assert( pTerm->eOperator&(WO_IN|WO_EQ|WO_LT|WO_LE|WO_GT|WO_GE|WO_AUX) );

      if( op & (WO_LT|WO_LE|WO_GT|WO_GE)
       && sqlite3ExprIsVector(pTerm->pExpr->pRight) 
      ){
        testcase( j!=i );
        if( j<16 ) mNoOmit |= (1 << j);
        if( op==WO_LT ) pIdxCons[j].op = WO_LE;
        if( op==WO_GT ) pIdxCons[j].op = WO_GE;
      }
    }

    j++;
  }
  assert( j==nTerm );
  pIdxInfo->nConstraint = j;
  for(i=j=0; i<nOrderBy; i++){
    Expr *pExpr = pOrderBy->a[i].pExpr;
    if( sqlite3ExprIsConstant(pExpr) ) continue;
    assert( pExpr->op==TK_COLUMN
         || (pExpr->op==TK_COLLATE && pExpr->pLeft->op==TK_COLUMN
              && pExpr->iColumn==pExpr->pLeft->iColumn) );
    pIdxOrderBy[j].iColumn = pExpr->iColumn;
    pIdxOrderBy[j].desc = pOrderBy->a[i].fg.sortFlags & KEYINFO_ORDER_DESC;
    j++;
  }
  pIdxInfo->nOrderBy = j;

  *pmNoOmit = mNoOmit;
  return pIdxInfo;
}