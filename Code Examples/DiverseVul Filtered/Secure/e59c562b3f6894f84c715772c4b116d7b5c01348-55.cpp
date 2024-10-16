KeyInfo *sqlite3KeyInfoRef(KeyInfo *p){
  if( p ){
    assert( p->nRef>0 );
    p->nRef++;
  }
  return p;
}