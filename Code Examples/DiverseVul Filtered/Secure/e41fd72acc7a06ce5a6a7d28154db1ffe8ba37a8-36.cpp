static void nodeReference(RtreeNode *p){
  if( p ){
    assert( p->nRef>0 );
    p->nRef++;
  }
}