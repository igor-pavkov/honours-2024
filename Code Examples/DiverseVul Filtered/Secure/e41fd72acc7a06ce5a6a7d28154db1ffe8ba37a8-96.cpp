static int rtreeBestIndex(sqlite3_vtab *tab, sqlite3_index_info *pIdxInfo){
  Rtree *pRtree = (Rtree*)tab;
  int rc = SQLITE_OK;
  int ii;
  int bMatch = 0;                 /* True if there exists a MATCH constraint */
  i64 nRow;                       /* Estimated rows returned by this scan */

  int iIdx = 0;
  char zIdxStr[RTREE_MAX_DIMENSIONS*8+1];
  memset(zIdxStr, 0, sizeof(zIdxStr));

  /* Check if there exists a MATCH constraint - even an unusable one. If there
  ** is, do not consider the lookup-by-rowid plan as using such a plan would
  ** require the VDBE to evaluate the MATCH constraint, which is not currently
  ** possible. */
  for(ii=0; ii<pIdxInfo->nConstraint; ii++){
    if( pIdxInfo->aConstraint[ii].op==SQLITE_INDEX_CONSTRAINT_MATCH ){
      bMatch = 1;
    }
  }

  assert( pIdxInfo->idxStr==0 );
  for(ii=0; ii<pIdxInfo->nConstraint && iIdx<(int)(sizeof(zIdxStr)-1); ii++){
    struct sqlite3_index_constraint *p = &pIdxInfo->aConstraint[ii];

    if( bMatch==0 && p->usable 
     && p->iColumn==0 && p->op==SQLITE_INDEX_CONSTRAINT_EQ 
    ){
      /* We have an equality constraint on the rowid. Use strategy 1. */
      int jj;
      for(jj=0; jj<ii; jj++){
        pIdxInfo->aConstraintUsage[jj].argvIndex = 0;
        pIdxInfo->aConstraintUsage[jj].omit = 0;
      }
      pIdxInfo->idxNum = 1;
      pIdxInfo->aConstraintUsage[ii].argvIndex = 1;
      pIdxInfo->aConstraintUsage[jj].omit = 1;

      /* This strategy involves a two rowid lookups on an B-Tree structures
      ** and then a linear search of an R-Tree node. This should be 
      ** considered almost as quick as a direct rowid lookup (for which 
      ** sqlite uses an internal cost of 0.0). It is expected to return
      ** a single row.
      */ 
      pIdxInfo->estimatedCost = 30.0;
      pIdxInfo->estimatedRows = 1;
      pIdxInfo->idxFlags = SQLITE_INDEX_SCAN_UNIQUE;
      return SQLITE_OK;
    }

    if( p->usable
    && ((p->iColumn>0 && p->iColumn<=pRtree->nDim2)
        || p->op==SQLITE_INDEX_CONSTRAINT_MATCH)
    ){
      u8 op;
      switch( p->op ){
        case SQLITE_INDEX_CONSTRAINT_EQ:    op = RTREE_EQ;    break;
        case SQLITE_INDEX_CONSTRAINT_GT:    op = RTREE_GT;    break;
        case SQLITE_INDEX_CONSTRAINT_LE:    op = RTREE_LE;    break;
        case SQLITE_INDEX_CONSTRAINT_LT:    op = RTREE_LT;    break;
        case SQLITE_INDEX_CONSTRAINT_GE:    op = RTREE_GE;    break;
        case SQLITE_INDEX_CONSTRAINT_MATCH: op = RTREE_MATCH; break;
        default:                            op = 0;           break;
      }
      if( op ){
        zIdxStr[iIdx++] = op;
        zIdxStr[iIdx++] = (char)(p->iColumn - 1 + '0');
        pIdxInfo->aConstraintUsage[ii].argvIndex = (iIdx/2);
        pIdxInfo->aConstraintUsage[ii].omit = 1;
      }
    }
  }

  pIdxInfo->idxNum = 2;
  pIdxInfo->needToFreeIdxStr = 1;
  if( iIdx>0 && 0==(pIdxInfo->idxStr = sqlite3_mprintf("%s", zIdxStr)) ){
    return SQLITE_NOMEM;
  }

  nRow = pRtree->nRowEst >> (iIdx/2);
  pIdxInfo->estimatedCost = (double)6.0 * (double)nRow;
  pIdxInfo->estimatedRows = nRow;

  return rc;
}