void sqlite3WithPush(Parse *pParse, With *pWith, u8 bFree){
  assert( bFree==0 || (pParse->pWith==0 && pParse->pWithToFree==0) );
  if( pWith ){
    assert( pParse->pWith!=pWith );
    pWith->pOuter = pParse->pWith;
    pParse->pWith = pWith;
    if( bFree ) pParse->pWithToFree = pWith;
  }
}