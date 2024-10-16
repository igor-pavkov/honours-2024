static Expr *whereRightSubexprIsColumn(Expr *p){
  p = sqlite3ExprSkipCollateAndLikely(p->pRight);
  if( ALWAYS(p!=0) && p->op==TK_COLUMN && !ExprHasProperty(p, EP_FixedCol) ){
    return p;
  }
  return 0;
}