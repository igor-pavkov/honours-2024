static int nodeAcquire(
  Rtree *pRtree,             /* R-tree structure */
  i64 iNode,                 /* Node number to load */
  RtreeNode *pParent,        /* Either the parent node or NULL */
  RtreeNode **ppNode         /* OUT: Acquired node */
){
  int rc = SQLITE_OK;
  RtreeNode *pNode = 0;

  /* Check if the requested node is already in the hash table. If so,
  ** increase its reference count and return it.
  */
  if( (pNode = nodeHashLookup(pRtree, iNode))!=0 ){
    assert( !pParent || !pNode->pParent || pNode->pParent==pParent );
    if( pParent && !pNode->pParent ){
      if( nodeInParentChain(pNode, pParent) ){
        RTREE_IS_CORRUPT(pRtree);
        return SQLITE_CORRUPT_VTAB;
      }
      pParent->nRef++;
      pNode->pParent = pParent;
    }
    pNode->nRef++;
    *ppNode = pNode;
    return SQLITE_OK;
  }

  if( pRtree->pNodeBlob ){
    sqlite3_blob *pBlob = pRtree->pNodeBlob;
    pRtree->pNodeBlob = 0;
    rc = sqlite3_blob_reopen(pBlob, iNode);
    pRtree->pNodeBlob = pBlob;
    if( rc ){
      nodeBlobReset(pRtree);
      if( rc==SQLITE_NOMEM ) return SQLITE_NOMEM;
    }
  }
  if( pRtree->pNodeBlob==0 ){
    char *zTab = sqlite3_mprintf("%s_node", pRtree->zName);
    if( zTab==0 ) return SQLITE_NOMEM;
    rc = sqlite3_blob_open(pRtree->db, pRtree->zDb, zTab, "data", iNode, 0,
                           &pRtree->pNodeBlob);
    sqlite3_free(zTab);
  }
  if( rc ){
    nodeBlobReset(pRtree);
    *ppNode = 0;
    /* If unable to open an sqlite3_blob on the desired row, that can only
    ** be because the shadow tables hold erroneous data. */
    if( rc==SQLITE_ERROR ){
      rc = SQLITE_CORRUPT_VTAB;
      RTREE_IS_CORRUPT(pRtree);
    }
  }else if( pRtree->iNodeSize==sqlite3_blob_bytes(pRtree->pNodeBlob) ){
    pNode = (RtreeNode *)sqlite3_malloc64(sizeof(RtreeNode)+pRtree->iNodeSize);
    if( !pNode ){
      rc = SQLITE_NOMEM;
    }else{
      pNode->pParent = pParent;
      pNode->zData = (u8 *)&pNode[1];
      pNode->nRef = 1;
      pRtree->nNodeRef++;
      pNode->iNode = iNode;
      pNode->isDirty = 0;
      pNode->pNext = 0;
      rc = sqlite3_blob_read(pRtree->pNodeBlob, pNode->zData,
                             pRtree->iNodeSize, 0);
    }
  }

  /* If the root node was just loaded, set pRtree->iDepth to the height
  ** of the r-tree structure. A height of zero means all data is stored on
  ** the root node. A height of one means the children of the root node
  ** are the leaves, and so on. If the depth as specified on the root node
  ** is greater than RTREE_MAX_DEPTH, the r-tree structure must be corrupt.
  */
  if( pNode && iNode==1 ){
    pRtree->iDepth = readInt16(pNode->zData);
    if( pRtree->iDepth>RTREE_MAX_DEPTH ){
      rc = SQLITE_CORRUPT_VTAB;
      RTREE_IS_CORRUPT(pRtree);
    }
  }

  /* If no error has occurred so far, check if the "number of entries"
  ** field on the node is too large. If so, set the return code to 
  ** SQLITE_CORRUPT_VTAB.
  */
  if( pNode && rc==SQLITE_OK ){
    if( NCELL(pNode)>((pRtree->iNodeSize-4)/pRtree->nBytesPerCell) ){
      rc = SQLITE_CORRUPT_VTAB;
      RTREE_IS_CORRUPT(pRtree);
    }
  }

  if( rc==SQLITE_OK ){
    if( pNode!=0 ){
      nodeReference(pParent);
      nodeHashInsert(pRtree, pNode);
    }else{
      rc = SQLITE_CORRUPT_VTAB;
      RTREE_IS_CORRUPT(pRtree);
    }
    *ppNode = pNode;
  }else{
    if( pNode ){
      pRtree->nNodeRef--;
      sqlite3_free(pNode);
    }
    *ppNode = 0;
  }

  return rc;
}