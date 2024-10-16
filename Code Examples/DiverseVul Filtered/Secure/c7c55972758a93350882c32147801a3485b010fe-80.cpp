  static void  Ins_SPVTL( INS_ARG )
  {
    if ( INS_SxVTL( args[1],
                    args[0],
                    CUR.opcode,
                    &CUR.GS.projVector) == FAILURE )
      return;

    CUR.GS.dualVector = CUR.GS.projVector;
    COMPUTE_Funcs();
  }