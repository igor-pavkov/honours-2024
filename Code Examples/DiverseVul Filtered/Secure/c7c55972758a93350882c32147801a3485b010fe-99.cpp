  static void  Ins_DIV( INS_ARG )
  {
    if ( args[1] == 0 )
    {
      CUR.error = TT_Err_Divide_By_Zero;
      return;
    }

    args[0] = MulDiv_Round( args[0], 64L, args[1] );
    DBG_PRINT1(" %d", args[0]);
  }