  static void  Ins_EVEN( INS_ARG )
  {
    if ( (CUR_Func_round( args[0], 0L ) & 127) == 0 )
      args[0] = 1;
    else
      args[0] = 0;
  }