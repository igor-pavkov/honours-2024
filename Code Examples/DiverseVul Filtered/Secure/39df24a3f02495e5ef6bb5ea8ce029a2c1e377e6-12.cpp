static void findConstInWhere(WhereConst *pConst, Expr *pExpr){
  Expr *pRight, *pLeft;
  if( pExpr==0 ) return;
  if( ExprHasProperty(pExpr, EP_FromJoin) ) return;
  if( pExpr->op==TK_AND ){
    findConstInWhere(pConst, pExpr->pRight);
    findConstInWhere(pConst, pExpr->pLeft);
    return;
  }
  if( pExpr->op!=TK_EQ ) return;
  pRight = pExpr->pRight;
  pLeft = pExpr->pLeft;
  assert( pRight!=0 );
  assert( pLeft!=0 );
  if( pRight->op==TK_COLUMN && sqlite3ExprIsConstant(pLeft) ){
    constInsert(pConst,pRight,pLeft,pExpr);
  }
  if( pLeft->op==TK_COLUMN && sqlite3ExprIsConstant(pRight) ){
    constInsert(pConst,pLeft,pRight,pExpr);
  }
}