  static void  Ins_SPVTCA( INS_ARG )
  {
    Short  A, B;
    (void)args;
    if ( CUR.opcode & 1 )
      A = 0x4000;
    else
      A = 0;

    B = A ^ 0x4000;

    CUR.GS.projVector.x = A;
    CUR.GS.dualVector.x = A;

    CUR.GS.projVector.y = B;
    CUR.GS.dualVector.y = B;

    COMPUTE_Funcs();
  }