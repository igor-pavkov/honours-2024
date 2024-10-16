static void showAllWhereLoops(WhereInfo *pWInfo, WhereClause *pWC){
  if( sqlite3WhereTrace ){    /* Display all of the WhereLoop objects */
    WhereLoop *p;
    int i;
    static const char zLabel[] = "0123456789abcdefghijklmnopqrstuvwyxz"
                                           "ABCDEFGHIJKLMNOPQRSTUVWYXZ";
    for(p=pWInfo->pLoops, i=0; p; p=p->pNextLoop, i++){
      p->cId = zLabel[i%(sizeof(zLabel)-1)];
      sqlite3WhereLoopPrint(p, pWC);
    }
  }
}