  static void  Ins_ROUND( INS_ARG )
  {
    args[0] = CUR_Func_round( args[0],
                              CUR.metrics.compensations[CUR.opcode - 0x68] );
  }