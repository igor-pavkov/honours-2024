static void addWhereTerm(
  Parse *pParse,                  /* Parsing context */
  SrcList *pSrc,                  /* List of tables in FROM clause */
  int iLeft,                      /* Index of first table to join in pSrc */
  int iColLeft,                   /* Index of column in first table */
  int iRight,                     /* Index of second table in pSrc */
  int iColRight,                  /* Index of column in second table */
  int isOuterJoin,                /* True if this is an OUTER join */
  Expr **ppWhere                  /* IN/OUT: The WHERE clause to add to */
){
  sqlite3 *db = pParse->db;
  Expr *pE1;
  Expr *pE2;
  Expr *pEq;

  assert( iLeft<iRight );
  assert( pSrc->nSrc>iRight );
  assert( pSrc->a[iLeft].pTab );
  assert( pSrc->a[iRight].pTab );

  pE1 = sqlite3CreateColumnExpr(db, pSrc, iLeft, iColLeft);
  pE2 = sqlite3CreateColumnExpr(db, pSrc, iRight, iColRight);

  pEq = sqlite3PExpr(pParse, TK_EQ, pE1, pE2);
  if( pEq && isOuterJoin ){
    ExprSetProperty(pEq, EP_FromJoin);
    assert( !ExprHasProperty(pEq, EP_TokenOnly|EP_Reduced) );
    ExprSetVVAProperty(pEq, EP_NoReduce);
    pEq->iRightJoinTable = (i16)pE2->iTable;
  }
  *ppWhere = sqlite3ExprAnd(pParse, *ppWhere, pEq);
}