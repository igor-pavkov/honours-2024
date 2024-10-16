static int indexColumnNotNull(Index *pIdx, int iCol){
  int j;
  assert( pIdx!=0 );
  assert( iCol>=0 && iCol<pIdx->nColumn );
  j = pIdx->aiColumn[iCol];
  if( j>=0 ){
    return pIdx->pTable->aCol[j].notNull;
  }else if( j==(-1) ){
    return 1;
  }else{
    assert( j==(-2) );
    return 0;  /* Assume an indexed expression can always yield a NULL */

  }
}