void sqlite3WhereClausePrint(WhereClause *pWC){
  int i;
  for(i=0; i<pWC->nTerm; i++){
    sqlite3WhereTermPrint(&pWC->a[i], i);
  }
}