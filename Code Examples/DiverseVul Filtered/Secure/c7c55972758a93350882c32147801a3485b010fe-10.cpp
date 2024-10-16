  static void  Ins_SCFS( INS_ARG )
  {
    Long  K;
    Int   L;

    L = (Int)args[0];

    if ( BOUNDS( args[0], CUR.zp2.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    K = CUR_Func_project( CUR.zp2.cur_x[L],
                          CUR.zp2.cur_y[L] );

    CUR_Func_move( &CUR.zp2, L, args[1] - K );

    /* not part of the specs, but here for safety */

    if ( CUR.GS.gep2 == 0 )
    {
      CUR.zp2.org_x[L] = CUR.zp2.cur_x[L];
      CUR.zp2.org_y[L] = CUR.zp2.cur_y[L];
    }
  }