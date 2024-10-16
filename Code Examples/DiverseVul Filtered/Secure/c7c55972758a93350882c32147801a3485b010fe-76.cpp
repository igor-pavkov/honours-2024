  static void  Ins_SWAP( INS_ARG )
  {
    Long  L;
    (void)exc;

    L       = args[0];
    args[0] = args[1];
    args[1] = L;
  }