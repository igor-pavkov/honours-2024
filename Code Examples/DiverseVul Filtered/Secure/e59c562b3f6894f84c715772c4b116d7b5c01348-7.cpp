static struct Cte *searchWith(
  With *pWith,                    /* Current innermost WITH clause */
  struct SrcList_item *pItem,     /* FROM clause element to resolve */
  With **ppContext                /* OUT: WITH clause return value belongs to */
){
  const char *zName;
  if( pItem->zDatabase==0 && (zName = pItem->zName)!=0 ){
    With *p;
    for(p=pWith; p; p=p->pOuter){
      int i;
      for(i=0; i<p->nCte; i++){
        if( sqlite3StrICmp(zName, p->a[i].zName)==0 ){
          *ppContext = p;
          return &p->a[i];
        }
      }
    }
  }
  return 0;
}