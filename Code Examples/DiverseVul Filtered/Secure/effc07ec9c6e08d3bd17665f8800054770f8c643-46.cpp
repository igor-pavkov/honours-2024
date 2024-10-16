static int cursorIsOpen(Vdbe *v, int iCur, int k){
  while( k>=0 ){
    VdbeOp *pOp = sqlite3VdbeGetOp(v,k--);
    if( pOp->p1!=iCur ) continue;
    if( pOp->opcode==OP_Close ) return 0;
    if( pOp->opcode==OP_OpenRead ) return 1;
    if( pOp->opcode==OP_OpenWrite ) return 1;
    if( pOp->opcode==OP_OpenDup ) return 1;
    if( pOp->opcode==OP_OpenAutoindex ) return 1;
    if( pOp->opcode==OP_OpenEphemeral ) return 1;
  }
  return 0;
}