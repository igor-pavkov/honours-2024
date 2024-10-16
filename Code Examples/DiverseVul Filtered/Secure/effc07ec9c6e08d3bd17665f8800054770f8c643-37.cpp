static void whereUndoExprMods(WhereInfo *pWInfo){
  while( pWInfo->pExprMods ){
    WhereExprMod *p = pWInfo->pExprMods;
    pWInfo->pExprMods = p->pNext;
    memcpy(p->pExpr, &p->orig, sizeof(p->orig));
    sqlite3DbFree(pWInfo->pParse->db, p);
  }
}