static int whereLoopAddVirtualOne(
  WhereLoopBuilder *pBuilder,
  Bitmask mPrereq,                /* Mask of tables that must be used. */
  Bitmask mUsable,                /* Mask of usable tables */
  u16 mExclude,                   /* Exclude terms using these operators */
  sqlite3_index_info *pIdxInfo,   /* Populated object for xBestIndex */
  u16 mNoOmit,                    /* Do not omit these constraints */
  int *pbIn,                      /* OUT: True if plan uses an IN(...) op */
  int *pbRetryLimit               /* OUT: Retry without LIMIT/OFFSET */
){
  WhereClause *pWC = pBuilder->pWC;
  HiddenIndexInfo *pHidden = (HiddenIndexInfo*)&pIdxInfo[1];
  struct sqlite3_index_constraint *pIdxCons;
  struct sqlite3_index_constraint_usage *pUsage = pIdxInfo->aConstraintUsage;
  int i;
  int mxTerm;
  int rc = SQLITE_OK;
  WhereLoop *pNew = pBuilder->pNew;
  Parse *pParse = pBuilder->pWInfo->pParse;
  SrcItem *pSrc = &pBuilder->pWInfo->pTabList->a[pNew->iTab];
  int nConstraint = pIdxInfo->nConstraint;

  assert( (mUsable & mPrereq)==mPrereq );
  *pbIn = 0;
  pNew->prereq = mPrereq;

  /* Set the usable flag on the subset of constraints identified by 
  ** arguments mUsable and mExclude. */
  pIdxCons = *(struct sqlite3_index_constraint**)&pIdxInfo->aConstraint;
  for(i=0; i<nConstraint; i++, pIdxCons++){
    WhereTerm *pTerm = &pWC->a[pIdxCons->iTermOffset];
    pIdxCons->usable = 0;
    if( (pTerm->prereqRight & mUsable)==pTerm->prereqRight 
     && (pTerm->eOperator & mExclude)==0
     && (pbRetryLimit || !isLimitTerm(pTerm))
    ){
      pIdxCons->usable = 1;
    }
  }

  /* Initialize the output fields of the sqlite3_index_info structure */
  memset(pUsage, 0, sizeof(pUsage[0])*nConstraint);
  assert( pIdxInfo->needToFreeIdxStr==0 );
  pIdxInfo->idxStr = 0;
  pIdxInfo->idxNum = 0;
  pIdxInfo->orderByConsumed = 0;
  pIdxInfo->estimatedCost = SQLITE_BIG_DBL / (double)2;
  pIdxInfo->estimatedRows = 25;
  pIdxInfo->idxFlags = 0;
  pIdxInfo->colUsed = (sqlite3_int64)pSrc->colUsed;
  pHidden->mHandleIn = 0;

  /* Invoke the virtual table xBestIndex() method */
  rc = vtabBestIndex(pParse, pSrc->pTab, pIdxInfo);
  if( rc ){
    if( rc==SQLITE_CONSTRAINT ){
      /* If the xBestIndex method returns SQLITE_CONSTRAINT, that means
      ** that the particular combination of parameters provided is unusable.
      ** Make no entries in the loop table.
      */
      WHERETRACE(0xffff, ("  ^^^^--- non-viable plan rejected!\n"));
      return SQLITE_OK;
    }
    return rc;
  }

  mxTerm = -1;
  assert( pNew->nLSlot>=nConstraint );
  memset(pNew->aLTerm, 0, sizeof(pNew->aLTerm[0])*nConstraint );
  memset(&pNew->u.vtab, 0, sizeof(pNew->u.vtab));
  pIdxCons = *(struct sqlite3_index_constraint**)&pIdxInfo->aConstraint;
  for(i=0; i<nConstraint; i++, pIdxCons++){
    int iTerm;
    if( (iTerm = pUsage[i].argvIndex - 1)>=0 ){
      WhereTerm *pTerm;
      int j = pIdxCons->iTermOffset;
      if( iTerm>=nConstraint
       || j<0
       || j>=pWC->nTerm
       || pNew->aLTerm[iTerm]!=0
       || pIdxCons->usable==0
      ){
        sqlite3ErrorMsg(pParse,"%s.xBestIndex malfunction",pSrc->pTab->zName);
        testcase( pIdxInfo->needToFreeIdxStr );
        return SQLITE_ERROR;
      }
      testcase( iTerm==nConstraint-1 );
      testcase( j==0 );
      testcase( j==pWC->nTerm-1 );
      pTerm = &pWC->a[j];
      pNew->prereq |= pTerm->prereqRight;
      assert( iTerm<pNew->nLSlot );
      pNew->aLTerm[iTerm] = pTerm;
      if( iTerm>mxTerm ) mxTerm = iTerm;
      testcase( iTerm==15 );
      testcase( iTerm==16 );
      if( pUsage[i].omit ){
        if( i<16 && ((1<<i)&mNoOmit)==0 ){
          testcase( i!=iTerm );
          pNew->u.vtab.omitMask |= 1<<iTerm;
        }else{
          testcase( i!=iTerm );
        }
        if( pTerm->eMatchOp==SQLITE_INDEX_CONSTRAINT_OFFSET ){
          pNew->u.vtab.bOmitOffset = 1;
        }
      }
      if( SMASKBIT32(i) & pHidden->mHandleIn ){ 
        pNew->u.vtab.mHandleIn |= MASKBIT32(iTerm);
      }else if( (pTerm->eOperator & WO_IN)!=0 ){
        /* A virtual table that is constrained by an IN clause may not
        ** consume the ORDER BY clause because (1) the order of IN terms
        ** is not necessarily related to the order of output terms and
        ** (2) Multiple outputs from a single IN value will not merge
        ** together.  */
        pIdxInfo->orderByConsumed = 0;
        pIdxInfo->idxFlags &= ~SQLITE_INDEX_SCAN_UNIQUE;
        *pbIn = 1; assert( (mExclude & WO_IN)==0 );
      }

      assert( pbRetryLimit || !isLimitTerm(pTerm) );
      if( isLimitTerm(pTerm) && *pbIn ){
        /* If there is an IN(...) term handled as an == (separate call to
        ** xFilter for each value on the RHS of the IN) and a LIMIT or
        ** OFFSET term handled as well, the plan is unusable. Set output
        ** variable *pbRetryLimit to true to tell the caller to retry with
        ** LIMIT and OFFSET disabled. */
        if( pIdxInfo->needToFreeIdxStr ){
          sqlite3_free(pIdxInfo->idxStr);
          pIdxInfo->idxStr = 0;
          pIdxInfo->needToFreeIdxStr = 0;
        }
        *pbRetryLimit = 1;
        return SQLITE_OK;
      }
    }
  }

  pNew->nLTerm = mxTerm+1;
  for(i=0; i<=mxTerm; i++){
    if( pNew->aLTerm[i]==0 ){
      /* The non-zero argvIdx values must be contiguous.  Raise an
      ** error if they are not */
      sqlite3ErrorMsg(pParse,"%s.xBestIndex malfunction",pSrc->pTab->zName);
      testcase( pIdxInfo->needToFreeIdxStr );
      return SQLITE_ERROR;
    }
  }
  assert( pNew->nLTerm<=pNew->nLSlot );
  pNew->u.vtab.idxNum = pIdxInfo->idxNum;
  pNew->u.vtab.needFree = pIdxInfo->needToFreeIdxStr;
  pIdxInfo->needToFreeIdxStr = 0;
  pNew->u.vtab.idxStr = pIdxInfo->idxStr;
  pNew->u.vtab.isOrdered = (i8)(pIdxInfo->orderByConsumed ?
      pIdxInfo->nOrderBy : 0);
  pNew->rSetup = 0;
  pNew->rRun = sqlite3LogEstFromDouble(pIdxInfo->estimatedCost);
  pNew->nOut = sqlite3LogEst(pIdxInfo->estimatedRows);

  /* Set the WHERE_ONEROW flag if the xBestIndex() method indicated
  ** that the scan will visit at most one row. Clear it otherwise. */
  if( pIdxInfo->idxFlags & SQLITE_INDEX_SCAN_UNIQUE ){
    pNew->wsFlags |= WHERE_ONEROW;
  }else{
    pNew->wsFlags &= ~WHERE_ONEROW;
  }
  rc = whereLoopInsert(pBuilder, pNew);
  if( pNew->u.vtab.needFree ){
    sqlite3_free(pNew->u.vtab.idxStr);
    pNew->u.vtab.needFree = 0;
  }
  WHERETRACE(0xffff, ("  bIn=%d prereqIn=%04llx prereqOut=%04llx\n",
                      *pbIn, (sqlite3_uint64)mPrereq,
                      (sqlite3_uint64)(pNew->prereq & ~mPrereq)));

  return rc;
}