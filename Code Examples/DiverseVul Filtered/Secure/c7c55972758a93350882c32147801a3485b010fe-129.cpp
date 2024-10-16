  static void  Ins_WCVTF( INS_ARG )
  {
#ifdef DEBUG
    int ov;
#endif

    if ( BOUNDS( args[0], CUR.cvtSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

#ifdef DEBUG
    ov = CUR.cvt[args[0]];
#endif
    CUR.cvt[args[0]] = FUnits_To_Pixels( EXEC_ARGS args[1] );
#ifdef DEBUG
    DBG_PRINT3(" cvt[%d]%d:=%d", args[0], ov, CUR.cvt[args[0]]);
#endif
  }