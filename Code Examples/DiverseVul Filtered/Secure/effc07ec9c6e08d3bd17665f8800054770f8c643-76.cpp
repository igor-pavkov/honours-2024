static void translateColumnToCopy(
  Parse *pParse,      /* Parsing context */
  int iStart,         /* Translate from this opcode to the end */
  int iTabCur,        /* OP_Column/OP_Rowid references to this table */
  int iRegister,      /* The first column is in this register */
  int iAutoidxCur     /* If non-zero, cursor of autoindex being generated */
){
  Vdbe *v = pParse->pVdbe;
  VdbeOp *pOp = sqlite3VdbeGetOp(v, iStart);
  int iEnd = sqlite3VdbeCurrentAddr(v);
  if( pParse->db->mallocFailed ) return;
  for(; iStart<iEnd; iStart++, pOp++){
    if( pOp->p1!=iTabCur ) continue;
    if( pOp->opcode==OP_Column ){
      pOp->opcode = OP_Copy;
      pOp->p1 = pOp->p2 + iRegister;
      pOp->p2 = pOp->p3;
      pOp->p3 = 0;
      pOp->p5 = 2;  /* Cause the MEM_Subtype flag to be cleared */
    }else if( pOp->opcode==OP_Rowid ){
      pOp->opcode = OP_Sequence;
      pOp->p1 = iAutoidxCur;
#ifdef SQLITE_ALLOW_ROWID_IN_VIEW
      if( iAutoidxCur==0 ){
        pOp->opcode = OP_Null;
        pOp->p3 = 0;
      }
#endif
    }
  }
}