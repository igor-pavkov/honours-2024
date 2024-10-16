  static void  Ins_WS( INS_ARG )
  {
    if ( BOUNDS( args[0], CUR.storeSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.storage[args[0]] = args[1];
  }