  static void  Ins_CEILING( INS_ARG )
  { (void)exc;
    args[0] = (args[0] + 63) & (-64);
  }