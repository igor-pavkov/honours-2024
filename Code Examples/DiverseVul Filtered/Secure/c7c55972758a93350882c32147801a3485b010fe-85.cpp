  static void  Ins_CINDEX( INS_ARG )
  {
    Long  L;

    L = args[0];

    if ( L<0 || L > CUR.args )
      CUR.error = TT_Err_Invalid_Reference;
    else
      args[0] = CUR.stack[CUR.args - L];
  }