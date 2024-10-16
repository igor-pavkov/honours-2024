  static void  Ins_ODD( INS_ARG )
  {
    if ( (CUR_Func_round( args[0], 0L ) & 127) == 64 )
      args[0] = 1;
    else
      args[0] = 0;
  }