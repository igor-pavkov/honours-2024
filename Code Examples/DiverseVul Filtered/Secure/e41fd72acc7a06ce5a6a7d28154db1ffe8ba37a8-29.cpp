static void rtreeNonleafConstraint(
  RtreeConstraint *p,        /* The constraint to test */
  int eInt,                  /* True if RTree holds integer coordinates */
  u8 *pCellData,             /* Raw cell content as appears on disk */
  int *peWithin              /* Adjust downward, as appropriate */
){
  sqlite3_rtree_dbl val;     /* Coordinate value convert to a double */

  /* p->iCoord might point to either a lower or upper bound coordinate
  ** in a coordinate pair.  But make pCellData point to the lower bound.
  */
  pCellData += 8 + 4*(p->iCoord&0xfe);

  assert(p->op==RTREE_LE || p->op==RTREE_LT || p->op==RTREE_GE 
      || p->op==RTREE_GT || p->op==RTREE_EQ );
  assert( ((((char*)pCellData) - (char*)0)&3)==0 );  /* 4-byte aligned */
  switch( p->op ){
    case RTREE_LE:
    case RTREE_LT:
    case RTREE_EQ:
      RTREE_DECODE_COORD(eInt, pCellData, val);
      /* val now holds the lower bound of the coordinate pair */
      if( p->u.rValue>=val ) return;
      if( p->op!=RTREE_EQ ) break;  /* RTREE_LE and RTREE_LT end here */
      /* Fall through for the RTREE_EQ case */

    default: /* RTREE_GT or RTREE_GE,  or fallthrough of RTREE_EQ */
      pCellData += 4;
      RTREE_DECODE_COORD(eInt, pCellData, val);
      /* val now holds the upper bound of the coordinate pair */
      if( p->u.rValue<=val ) return;
  }
  *peWithin = NOT_WITHIN;
}