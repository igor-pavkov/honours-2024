  static void  Ins_NROUND( INS_ARG )
  {
    args[0] = Round_None( EXEC_ARGS
                          args[0],
                          CUR.metrics.compensations[CUR.opcode - 0x6C] );
  }