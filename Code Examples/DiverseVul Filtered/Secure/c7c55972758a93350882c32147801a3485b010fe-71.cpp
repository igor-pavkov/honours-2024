  static void  Ins_MUL( INS_ARG )
  { (void)exc;
    args[0] = MulDiv_Round( args[0], args[1], 64L );
  }