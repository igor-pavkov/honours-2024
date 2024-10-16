  static void  Ins_RCVT( INS_ARG )
  {
    int index;
    if ( BOUNDS( args[0], CUR.cvtSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }
    index=args[0];
    args[0] = CUR_Func_read_cvt( index );
    DBG_PRINT3(" cvt[%d]%d:%d", index, CUR.cvt[index], args[0]);
  }