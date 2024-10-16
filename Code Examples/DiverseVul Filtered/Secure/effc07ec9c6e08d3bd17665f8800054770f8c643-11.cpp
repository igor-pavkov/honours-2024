static SQLITE_NOINLINE void whereCheckIfBloomFilterIsUseful(
  const WhereInfo *pWInfo
){
  int i;
  LogEst nSearch;

  assert( pWInfo->nLevel>=2 );
  assert( OptimizationEnabled(pWInfo->pParse->db, SQLITE_BloomFilter) );
  nSearch = pWInfo->a[0].pWLoop->nOut;
  for(i=1; i<pWInfo->nLevel; i++){
    WhereLoop *pLoop = pWInfo->a[i].pWLoop;
    const unsigned int reqFlags = (WHERE_SELFCULL|WHERE_COLUMN_EQ);
    if( (pLoop->wsFlags & reqFlags)==reqFlags
     /* vvvvvv--- Always the case if WHERE_COLUMN_EQ is defined */
     && ALWAYS((pLoop->wsFlags & (WHERE_IPK|WHERE_INDEXED))!=0)
    ){
      SrcItem *pItem = &pWInfo->pTabList->a[pLoop->iTab];
      Table *pTab = pItem->pTab;
      pTab->tabFlags |= TF_StatsUsed;
      if( nSearch > pTab->nRowLogEst
       && (pTab->tabFlags & TF_HasStat1)!=0
      ){
        testcase( pItem->fg.jointype & JT_LEFT );
        pLoop->wsFlags |= WHERE_BLOOMFILTER;
        pLoop->wsFlags &= ~WHERE_IDX_ONLY;
        WHERETRACE(0xffff, (
           "-> use Bloom-filter on loop %c because there are ~%.1e "
           "lookups into %s which has only ~%.1e rows\n",
           pLoop->cId, (double)sqlite3LogEstToInt(nSearch), pTab->zName,
           (double)sqlite3LogEstToInt(pTab->nRowLogEst)));
      }
    }
    nSearch += pLoop->nOut;
  }
}