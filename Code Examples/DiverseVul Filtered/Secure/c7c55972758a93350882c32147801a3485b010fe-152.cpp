  static void  Ins_RS( INS_ARG )
  {
    if ( BOUNDS( args[0], CUR.storeSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    args[0] = CUR.storage[args[0]];
  }