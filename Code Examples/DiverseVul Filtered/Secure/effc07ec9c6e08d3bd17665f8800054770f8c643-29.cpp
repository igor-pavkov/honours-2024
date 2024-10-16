static LogEst whereSortingCost(
  WhereInfo *pWInfo,
  LogEst nRow,
  int nOrderBy,
  int nSorted
){
  /* TUNING: Estimated cost of a full external sort, where N is 
  ** the number of rows to sort is:
  **
  **   cost = (3.0 * N * log(N)).
  ** 
  ** Or, if the order-by clause has X terms but only the last Y 
  ** terms are out of order, then block-sorting will reduce the 
  ** sorting cost to:
  **
  **   cost = (3.0 * N * log(N)) * (Y/X)
  **
  ** The (Y/X) term is implemented using stack variable rScale
  ** below.
  */
  LogEst rScale, rSortCost;
  assert( nOrderBy>0 && 66==sqlite3LogEst(100) );
  rScale = sqlite3LogEst((nOrderBy-nSorted)*100/nOrderBy) - 66;
  rSortCost = nRow + rScale + 16;

  /* Multiple by log(M) where M is the number of output rows.
  ** Use the LIMIT for M if it is smaller.  Or if this sort is for
  ** a DISTINCT operator, M will be the number of distinct output
  ** rows, so fudge it downwards a bit.
  */
  if( (pWInfo->wctrlFlags & WHERE_USE_LIMIT)!=0 && pWInfo->iLimit<nRow ){
    nRow = pWInfo->iLimit;
  }else if( (pWInfo->wctrlFlags & WHERE_WANT_DISTINCT) ){
    /* TUNING: In the sort for a DISTINCT operator, assume that the DISTINCT
    ** reduces the number of output rows by a factor of 2 */
    if( nRow>10 ){ nRow -= 10;  assert( 10==sqlite3LogEst(2) ); }
  }
  rSortCost += estLog(nRow);
  return rSortCost;
}