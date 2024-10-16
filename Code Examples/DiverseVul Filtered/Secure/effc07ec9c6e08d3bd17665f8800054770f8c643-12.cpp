static int termCanDriveIndex(
  const WhereTerm *pTerm,        /* WHERE clause term to check */
  const SrcItem *pSrc,           /* Table we are trying to access */
  const Bitmask notReady         /* Tables in outer loops of the join */
){
  char aff;
  if( pTerm->leftCursor!=pSrc->iCursor ) return 0;
  if( (pTerm->eOperator & (WO_EQ|WO_IS))==0 ) return 0;
  assert( (pSrc->fg.jointype & JT_RIGHT)==0 );
  if( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))!=0 ){
    testcase( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))==JT_LEFT );
    testcase( (pSrc->fg.jointype & (JT_LEFT|JT_LTORJ|JT_RIGHT))==JT_LTORJ );
    testcase( ExprHasProperty(pTerm->pExpr, EP_OuterON) )
    testcase( ExprHasProperty(pTerm->pExpr, EP_InnerON) );
    if( !ExprHasProperty(pTerm->pExpr, EP_OuterON|EP_InnerON)
     || pTerm->pExpr->w.iJoin != pSrc->iCursor
    ){
      return 0;  /* See tag-20191211-001 */
    }
  }
  if( (pTerm->prereqRight & notReady)!=0 ) return 0;
  assert( (pTerm->eOperator & (WO_OR|WO_AND))==0 );
  if( pTerm->u.x.leftColumn<0 ) return 0;
  aff = pSrc->pTab->aCol[pTerm->u.x.leftColumn].affinity;
  if( !sqlite3IndexAffinityOk(pTerm->pExpr, aff) ) return 0;
  testcase( pTerm->pExpr->op==TK_IS );
  return 1;
}