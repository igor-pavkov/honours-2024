  static void  Ins_ROLL( INS_ARG )
  {
    Long  A, B, C;
    (void)exc;

    A = args[2];
    B = args[1];
    C = args[0];

    args[2] = C;
    args[1] = A;
    args[0] = B;
  }