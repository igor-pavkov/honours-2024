void sqlite3WhereTermPrint(WhereTerm *pTerm, int iTerm){
  if( pTerm==0 ){
    sqlite3DebugPrintf("TERM-%-3d NULL\n", iTerm);
  }else{
    char zType[8];
    char zLeft[50];
    memcpy(zType, "....", 5);
    if( pTerm->wtFlags & TERM_VIRTUAL ) zType[0] = 'V';
    if( pTerm->eOperator & WO_EQUIV  ) zType[1] = 'E';
    if( ExprHasProperty(pTerm->pExpr, EP_OuterON) ) zType[2] = 'L';
    if( pTerm->wtFlags & TERM_CODED  ) zType[3] = 'C';
    if( pTerm->eOperator & WO_SINGLE ){
      assert( (pTerm->eOperator & (WO_OR|WO_AND))==0 );
      sqlite3_snprintf(sizeof(zLeft),zLeft,"left={%d:%d}",
                       pTerm->leftCursor, pTerm->u.x.leftColumn);
    }else if( (pTerm->eOperator & WO_OR)!=0 && pTerm->u.pOrInfo!=0 ){
      sqlite3_snprintf(sizeof(zLeft),zLeft,"indexable=0x%llx", 
                       pTerm->u.pOrInfo->indexable);
    }else{
      sqlite3_snprintf(sizeof(zLeft),zLeft,"left=%d", pTerm->leftCursor);
    }
    sqlite3DebugPrintf(
       "TERM-%-3d %p %s %-12s op=%03x wtFlags=%04x",
       iTerm, pTerm, zType, zLeft, pTerm->eOperator, pTerm->wtFlags);
    /* The 0x10000 .wheretrace flag causes extra information to be
    ** shown about each Term */
    if( sqlite3WhereTrace & 0x10000 ){
      sqlite3DebugPrintf(" prob=%-3d prereq=%llx,%llx",
        pTerm->truthProb, (u64)pTerm->prereqAll, (u64)pTerm->prereqRight);
    }
    if( (pTerm->eOperator & (WO_OR|WO_AND))==0 && pTerm->u.x.iField ){
      sqlite3DebugPrintf(" iField=%d", pTerm->u.x.iField);
    }
    if( pTerm->iParent>=0 ){
      sqlite3DebugPrintf(" iParent=%d", pTerm->iParent);
    }
    sqlite3DebugPrintf("\n");
    sqlite3TreeViewExpr(0, pTerm->pExpr, 0);
  }
}