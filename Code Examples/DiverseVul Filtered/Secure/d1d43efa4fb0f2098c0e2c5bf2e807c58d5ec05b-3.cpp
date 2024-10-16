int sqlite3Fts5UnicodeCategory(u32 iCode) { 
  int iRes = -1;
  int iHi;
  int iLo;
  int ret;
  u16 iKey;

  if( iCode>=(1<<20) ){
    return 0;
  }
  iLo = aFts5UnicodeBlock[(iCode>>16)];
  iHi = aFts5UnicodeBlock[1+(iCode>>16)];
  iKey = (iCode & 0xFFFF);
  while( iHi>iLo ){
    int iTest = (iHi + iLo) / 2;
    assert( iTest>=iLo && iTest<iHi );
    if( iKey>=aFts5UnicodeMap[iTest] ){
      iRes = iTest;
      iLo = iTest+1;
    }else{
      iHi = iTest;
    }
  }

  if( iRes<0 ) return 0;
  if( iKey>=(aFts5UnicodeMap[iRes]+(aFts5UnicodeData[iRes]>>5)) ) return 0;
  ret = aFts5UnicodeData[iRes] & 0x1F;
  if( ret!=30 ) return ret;
  return ((iKey - aFts5UnicodeMap[iRes]) & 0x01) ? 5 : 9;
}