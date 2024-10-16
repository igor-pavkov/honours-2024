static void rtreeLeafConstraint(
  RtreeConstraint *p,        /* The constraint to test */
  int eInt,                  /* True if RTree holds integer coordinates */
  u8 *pCellData,             /* Raw cell content as appears on disk */
  int *peWithin              /* Adjust downward, as appropriate */
){
  RtreeDValue xN;      /* Coordinate value converted to a double */

  assert(p->op==RTREE_LE || p->op==RTREE_LT || p->op==RTREE_GE 
      || p->op==RTREE_GT || p->op==RTREE_EQ );
  pCellData += 8 + p->iCoord*4;
  assert( ((((char*)pCellData) - (char*)0)&3)==0 );  /* 4-byte aligned */
  RTREE_DECODE_COORD(eInt, pCellData, xN);
  switch( p->op ){
    case RTREE_LE: if( xN <= p->u.rValue ) return;  break;
    case RTREE_LT: if( xN <  p->u.rValue ) return;  break;
    case RTREE_GE: if( xN >= p->u.rValue ) return;  break;
    case RTREE_GT: if( xN >  p->u.rValue ) return;  break;
    default:       if( xN == p->u.rValue ) return;  break;
  }
  *peWithin = NOT_WITHIN;
}