static int rtreeCallbackConstraint(
  RtreeConstraint *pConstraint,  /* The constraint to test */
  int eInt,                      /* True if RTree holding integer coordinates */
  u8 *pCellData,                 /* Raw cell content */
  RtreeSearchPoint *pSearch,     /* Container of this cell */
  sqlite3_rtree_dbl *prScore,    /* OUT: score for the cell */
  int *peWithin                  /* OUT: visibility of the cell */
){
  sqlite3_rtree_query_info *pInfo = pConstraint->pInfo; /* Callback info */
  int nCoord = pInfo->nCoord;                           /* No. of coordinates */
  int rc;                                             /* Callback return code */
  RtreeCoord c;                                       /* Translator union */
  sqlite3_rtree_dbl aCoord[RTREE_MAX_DIMENSIONS*2];   /* Decoded coordinates */

  assert( pConstraint->op==RTREE_MATCH || pConstraint->op==RTREE_QUERY );
  assert( nCoord==2 || nCoord==4 || nCoord==6 || nCoord==8 || nCoord==10 );

  if( pConstraint->op==RTREE_QUERY && pSearch->iLevel==1 ){
    pInfo->iRowid = readInt64(pCellData);
  }
  pCellData += 8;
#ifndef SQLITE_RTREE_INT_ONLY
  if( eInt==0 ){
    switch( nCoord ){
      case 10:  readCoord(pCellData+36, &c); aCoord[9] = c.f;
                readCoord(pCellData+32, &c); aCoord[8] = c.f;
      case 8:   readCoord(pCellData+28, &c); aCoord[7] = c.f;
                readCoord(pCellData+24, &c); aCoord[6] = c.f;
      case 6:   readCoord(pCellData+20, &c); aCoord[5] = c.f;
                readCoord(pCellData+16, &c); aCoord[4] = c.f;
      case 4:   readCoord(pCellData+12, &c); aCoord[3] = c.f;
                readCoord(pCellData+8,  &c); aCoord[2] = c.f;
      default:  readCoord(pCellData+4,  &c); aCoord[1] = c.f;
                readCoord(pCellData,    &c); aCoord[0] = c.f;
    }
  }else
#endif
  {
    switch( nCoord ){
      case 10:  readCoord(pCellData+36, &c); aCoord[9] = c.i;
                readCoord(pCellData+32, &c); aCoord[8] = c.i;
      case 8:   readCoord(pCellData+28, &c); aCoord[7] = c.i;
                readCoord(pCellData+24, &c); aCoord[6] = c.i;
      case 6:   readCoord(pCellData+20, &c); aCoord[5] = c.i;
                readCoord(pCellData+16, &c); aCoord[4] = c.i;
      case 4:   readCoord(pCellData+12, &c); aCoord[3] = c.i;
                readCoord(pCellData+8,  &c); aCoord[2] = c.i;
      default:  readCoord(pCellData+4,  &c); aCoord[1] = c.i;
                readCoord(pCellData,    &c); aCoord[0] = c.i;
    }
  }
  if( pConstraint->op==RTREE_MATCH ){
    int eWithin = 0;
    rc = pConstraint->u.xGeom((sqlite3_rtree_geometry*)pInfo,
                              nCoord, aCoord, &eWithin);
    if( eWithin==0 ) *peWithin = NOT_WITHIN;
    *prScore = RTREE_ZERO;
  }else{
    pInfo->aCoord = aCoord;
    pInfo->iLevel = pSearch->iLevel - 1;
    pInfo->rScore = pInfo->rParentScore = pSearch->rScore;
    pInfo->eWithin = pInfo->eParentWithin = pSearch->eWithin;
    rc = pConstraint->u.xQueryFunc(pInfo);
    if( pInfo->eWithin<*peWithin ) *peWithin = pInfo->eWithin;
    if( pInfo->rScore<*prScore || *prScore<RTREE_ZERO ){
      *prScore = pInfo->rScore;
    }
  }
  return rc;
}