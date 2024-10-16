static int whereLoopAddAll(WhereLoopBuilder *pBuilder){
  WhereInfo *pWInfo = pBuilder->pWInfo;
  Bitmask mPrereq = 0;
  Bitmask mPrior = 0;
  int iTab;
  SrcList *pTabList = pWInfo->pTabList;
  SrcItem *pItem;
  SrcItem *pEnd = &pTabList->a[pWInfo->nLevel];
  sqlite3 *db = pWInfo->pParse->db;
  int rc = SQLITE_OK;
  int bFirstPastRJ = 0;
  WhereLoop *pNew;


  /* Loop over the tables in the join, from left to right */
  pNew = pBuilder->pNew;
  whereLoopInit(pNew);
  pBuilder->iPlanLimit = SQLITE_QUERY_PLANNER_LIMIT;
  for(iTab=0, pItem=pTabList->a; pItem<pEnd; iTab++, pItem++){
    Bitmask mUnusable = 0;
    pNew->iTab = iTab;
    pBuilder->iPlanLimit += SQLITE_QUERY_PLANNER_LIMIT_INCR;
    pNew->maskSelf = sqlite3WhereGetMask(&pWInfo->sMaskSet, pItem->iCursor);
    if( bFirstPastRJ 
     || (pItem->fg.jointype & (JT_OUTER|JT_CROSS|JT_LTORJ))!=0
    ){
      /* Add prerequisites to prevent reordering of FROM clause terms
      ** across CROSS joins and outer joins.  The bFirstPastRJ boolean
      ** prevents the right operand of a RIGHT JOIN from being swapped with
      ** other elements even further to the right.
      **
      ** The JT_LTORJ term prevents any FROM-clause term reordering for terms
      ** to the left of a RIGHT JOIN.  This is conservative.  Relaxing this
      ** constraint somewhat to prevent terms from crossing from the right
      ** side of a LEFT JOIN over to the left side when they are on the
      ** left side of a RIGHT JOIN would be sufficient for all known failure
      ** cases.  FIX ME: Implement this optimization.
      */
      mPrereq |= mPrior;
      bFirstPastRJ = (pItem->fg.jointype & JT_RIGHT)!=0;
    }
#ifndef SQLITE_OMIT_VIRTUALTABLE
    if( IsVirtual(pItem->pTab) ){
      SrcItem *p;
      for(p=&pItem[1]; p<pEnd; p++){
        if( mUnusable || (p->fg.jointype & (JT_OUTER|JT_CROSS)) ){
          mUnusable |= sqlite3WhereGetMask(&pWInfo->sMaskSet, p->iCursor);
        }
      }
      rc = whereLoopAddVirtual(pBuilder, mPrereq, mUnusable);
    }else
#endif /* SQLITE_OMIT_VIRTUALTABLE */
    {
      rc = whereLoopAddBtree(pBuilder, mPrereq);
    }
    if( rc==SQLITE_OK && pBuilder->pWC->hasOr ){
      rc = whereLoopAddOr(pBuilder, mPrereq, mUnusable);
    }
    mPrior |= pNew->maskSelf;
    if( rc || db->mallocFailed ){
      if( rc==SQLITE_DONE ){
        /* We hit the query planner search limit set by iPlanLimit */
        sqlite3_log(SQLITE_WARNING, "abbreviated query algorithm search");
        rc = SQLITE_OK;
      }else{
        break;
      }
    }
  }

  whereLoopClear(db, pNew);
  return rc;
}