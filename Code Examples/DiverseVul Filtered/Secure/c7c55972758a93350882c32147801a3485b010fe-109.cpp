  static void  Ins_OR( INS_ARG )
  { (void)exc;
    if ( args[0] != 0 || args[1] != 0 )
      args[0] = 1;
    else
      args[0] = 0;
  }