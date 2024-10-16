static Select *findRightmost(Select *p){
  while( p->pNext ) p = p->pNext;
  return p;
}