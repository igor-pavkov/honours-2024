void sqlite3WhereLoopPrint(WhereLoop *p, WhereClause *pWC){
  WhereInfo *pWInfo = pWC->pWInfo;
  int nb = 1+(pWInfo->pTabList->nSrc+3)/4;
  SrcItem *pItem = pWInfo->pTabList->a + p->iTab;
  Table *pTab = pItem->pTab;
  Bitmask mAll = (((Bitmask)1)<<(nb*4)) - 1;
  sqlite3DebugPrintf("%c%2d.%0*llx.%0*llx", p->cId,
                     p->iTab, nb, p->maskSelf, nb, p->prereq & mAll);
  sqlite3DebugPrintf(" %12s",
                     pItem->zAlias ? pItem->zAlias : pTab->zName);
  if( (p->wsFlags & WHERE_VIRTUALTABLE)==0 ){
    const char *zName;
    if( p->u.btree.pIndex && (zName = p->u.btree.pIndex->zName)!=0 ){
      if( strncmp(zName, "sqlite_autoindex_", 17)==0 ){
        int i = sqlite3Strlen30(zName) - 1;
        while( zName[i]!='_' ) i--;
        zName += i;
      }
      sqlite3DebugPrintf(".%-16s %2d", zName, p->u.btree.nEq);
    }else{
      sqlite3DebugPrintf("%20s","");
    }
  }else{
    char *z;
    if( p->u.vtab.idxStr ){
      z = sqlite3_mprintf("(%d,\"%s\",%#x)",
                p->u.vtab.idxNum, p->u.vtab.idxStr, p->u.vtab.omitMask);
    }else{
      z = sqlite3_mprintf("(%d,%x)", p->u.vtab.idxNum, p->u.vtab.omitMask);
    }
    sqlite3DebugPrintf(" %-19s", z);
    sqlite3_free(z);
  }
  if( p->wsFlags & WHERE_SKIPSCAN ){
    sqlite3DebugPrintf(" f %06x %d-%d", p->wsFlags, p->nLTerm,p->nSkip);
  }else{
    sqlite3DebugPrintf(" f %06x N %d", p->wsFlags, p->nLTerm);
  }
  sqlite3DebugPrintf(" cost %d,%d,%d\n", p->rSetup, p->rRun, p->nOut);
  if( p->nLTerm && (sqlite3WhereTrace & 0x100)!=0 ){
    int i;
    for(i=0; i<p->nLTerm; i++){
      sqlite3WhereTermPrint(p->aLTerm[i], i);
    }
  }
}