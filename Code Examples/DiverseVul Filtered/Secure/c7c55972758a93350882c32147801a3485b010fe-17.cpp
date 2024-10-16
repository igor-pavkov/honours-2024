  static void  Ins_MIN( INS_ARG )
  { (void)exc;
    if ( args[1] < args[0] )
      args[0] = args[1];
  }