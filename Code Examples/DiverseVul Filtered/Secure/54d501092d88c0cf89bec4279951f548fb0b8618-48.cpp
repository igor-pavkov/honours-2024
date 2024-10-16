static void zipfileDequote(char *zIn){
  char q = zIn[0];
  if( q=='"' || q=='\'' || q=='`' || q=='[' ){
    int iIn = 1;
    int iOut = 0;
    if( q=='[' ) q = ']';
    while( ALWAYS(zIn[iIn]) ){
      char c = zIn[iIn++];
      if( c==q && zIn[iIn++]!=q ) break;
      zIn[iOut++] = c;
    }
    zIn[iOut] = '\0';
  }
}