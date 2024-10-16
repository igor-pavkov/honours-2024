static int isLimitTerm(WhereTerm *pTerm){
  assert( pTerm->eOperator==WO_AUX || pTerm->eMatchOp==0 );
  return pTerm->eMatchOp>=SQLITE_INDEX_CONSTRAINT_LIMIT 
      && pTerm->eMatchOp<=SQLITE_INDEX_CONSTRAINT_OFFSET;
}