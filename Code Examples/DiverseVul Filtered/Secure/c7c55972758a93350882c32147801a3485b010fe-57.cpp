  static void  Ins_NOT( INS_ARG )
  { (void)exc;
    if ( args[0] != 0 )
      args[0] = 0;
    else
      args[0] = 1;
  }