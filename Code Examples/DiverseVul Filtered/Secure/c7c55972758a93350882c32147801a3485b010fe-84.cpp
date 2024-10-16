  static void  Ins_MINDEX( INS_ARG )
  {
    Long  L, K;

    L = args[0];

    if (L == 0)
        return;

    if ( L<0 || L > CUR.args )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    K = CUR.stack[CUR.args - L];

    memmove( (&CUR.stack[CUR.args - L    ]),
              (&CUR.stack[CUR.args - L + 1]),
              (L - 1) * sizeof ( Long ) );

    CUR.stack[ CUR.args-1 ] = K;
  }