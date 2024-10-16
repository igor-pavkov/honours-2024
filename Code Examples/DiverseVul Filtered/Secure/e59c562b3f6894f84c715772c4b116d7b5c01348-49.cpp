static void unsetJoinExpr(Expr *p, int iTable){
  while( p ){
    if( ExprHasProperty(p, EP_FromJoin)
     && (iTable<0 || p->iRightJoinTable==iTable) ){
      ExprClearProperty(p, EP_FromJoin);
    }
    if( p->op==TK_FUNCTION && p->x.pList ){
      int i;
      for(i=0; i<p->x.pList->nExpr; i++){
        unsetJoinExpr(p->x.pList->a[i].pExpr, iTable);
      }
    }
    unsetJoinExpr(p->pLeft, iTable);
    p = p->pRight;
  } 
}