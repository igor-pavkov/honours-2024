static int whereLoopCheaperProperSubset(
  const WhereLoop *pX,       /* First WhereLoop to compare */
  const WhereLoop *pY        /* Compare against this WhereLoop */
){
  int i, j;
  if( pX->nLTerm-pX->nSkip >= pY->nLTerm-pY->nSkip ){
    return 0; /* X is not a subset of Y */
  }
  if( pX->rRun>pY->rRun && pX->nOut>pY->nOut ) return 0;
  if( pY->nSkip > pX->nSkip ) return 0;
  for(i=pX->nLTerm-1; i>=0; i--){
    if( pX->aLTerm[i]==0 ) continue;
    for(j=pY->nLTerm-1; j>=0; j--){
      if( pY->aLTerm[j]==pX->aLTerm[i] ) break;
    }
    if( j<0 ) return 0;  /* X not a subset of Y since term X[i] not used by Y */
  }
  if( (pX->wsFlags&WHERE_IDX_ONLY)!=0 
   && (pY->wsFlags&WHERE_IDX_ONLY)==0 ){
    return 0;  /* Constraint (5) */
  }
  return 1;  /* All conditions meet */
}