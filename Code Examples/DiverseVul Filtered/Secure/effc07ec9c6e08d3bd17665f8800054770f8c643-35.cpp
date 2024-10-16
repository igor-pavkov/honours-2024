static int exprNodeIsDeterministic(Walker *pWalker, Expr *pExpr){
  if( pExpr->op==TK_FUNCTION && ExprHasProperty(pExpr, EP_ConstFunc)==0 ){
    pWalker->eCode = 0;
    return WRC_Abort;
  }
  return WRC_Continue;
}