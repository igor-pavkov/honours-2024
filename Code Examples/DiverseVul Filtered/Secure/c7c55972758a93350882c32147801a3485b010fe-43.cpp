  static void  Ins_SFVTL( INS_ARG )
  {
    if ( INS_SxVTL( (Int)(args[1]),
                    (Int)(args[0]),
                    CUR.opcode,
                    &CUR.GS.freeVector) == FAILURE )
      return;

    COMPUTE_Funcs();
  }