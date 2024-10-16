  static void  Ins_SFVTPV( INS_ARG )
  { (void)args;
    CUR.GS.freeVector = CUR.GS.projVector;
    COMPUTE_Funcs();
  }