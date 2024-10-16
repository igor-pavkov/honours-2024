static u8 *rtreeCheckGetNode(RtreeCheck *pCheck, i64 iNode, int *pnNode){
  u8 *pRet = 0;                   /* Return value */

  if( pCheck->rc==SQLITE_OK && pCheck->pGetNode==0 ){
    pCheck->pGetNode = rtreeCheckPrepare(pCheck,
        "SELECT data FROM %Q.'%q_node' WHERE nodeno=?", 
        pCheck->zDb, pCheck->zTab
    );
  }

  if( pCheck->rc==SQLITE_OK ){
    sqlite3_bind_int64(pCheck->pGetNode, 1, iNode);
    if( sqlite3_step(pCheck->pGetNode)==SQLITE_ROW ){
      int nNode = sqlite3_column_bytes(pCheck->pGetNode, 0);
      const u8 *pNode = (const u8*)sqlite3_column_blob(pCheck->pGetNode, 0);
      pRet = sqlite3_malloc64(nNode);
      if( pRet==0 ){
        pCheck->rc = SQLITE_NOMEM;
      }else{
        memcpy(pRet, pNode, nNode);
        *pnNode = nNode;
      }
    }
    rtreeCheckReset(pCheck, pCheck->pGetNode);
    if( pCheck->rc==SQLITE_OK && pRet==0 ){
      rtreeCheckAppendMsg(pCheck, "Node %lld missing from database", iNode);
    }
  }

  return pRet;
}