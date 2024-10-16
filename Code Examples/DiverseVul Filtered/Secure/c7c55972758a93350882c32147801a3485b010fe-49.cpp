  static void  Ins_WCVTP( INS_ARG )
  {
    if ( BOUNDS( args[0], CUR.cvtSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR_Func_write_cvt( args[0], args[1] );
  }