static LogEst whereRangeAdjust(WhereTerm *pTerm, LogEst nNew){
  LogEst nRet = nNew;
  if( pTerm ){
    if( pTerm->truthProb<=0 ){
      nRet += pTerm->truthProb;
    }else if( (pTerm->wtFlags & TERM_VNULL)==0 ){
      nRet -= 20;        assert( 20==sqlite3LogEst(4) );
    }
  }
  return nRet;
}