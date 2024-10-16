Bitmask sqlite3WhereGetMask(WhereMaskSet *pMaskSet, int iCursor){
  int i;
  assert( pMaskSet->n<=(int)sizeof(Bitmask)*8 );
  assert( pMaskSet->n>0 || pMaskSet->ix[0]<0 );
  assert( iCursor>=-1 );
  if( pMaskSet->ix[0]==iCursor ){
    return 1;
  }
  for(i=1; i<pMaskSet->n; i++){
    if( pMaskSet->ix[i]==iCursor ){
      return MASKBIT(i);
    }
  }
  return 0;
}