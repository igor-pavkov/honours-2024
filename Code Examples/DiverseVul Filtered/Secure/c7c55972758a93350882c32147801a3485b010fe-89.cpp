  static void  Ins_MAX( INS_ARG )
  { (void)exc;
    if ( args[1] > args[0] )
      args[0] = args[1];
  }