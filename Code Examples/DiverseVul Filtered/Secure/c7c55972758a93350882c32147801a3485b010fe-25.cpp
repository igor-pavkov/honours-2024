  static void  Ins_SFVTCA( INS_ARG )
  {
    Short  A, B;
    (void)args;

    if ( CUR.opcode & 1 )
      A = 0x4000;
    else
      A = 0;

    B = A ^ 0x4000;

    CUR.GS.freeVector.x = A;
    CUR.GS.freeVector.y = B;

    COMPUTE_Funcs();
  }