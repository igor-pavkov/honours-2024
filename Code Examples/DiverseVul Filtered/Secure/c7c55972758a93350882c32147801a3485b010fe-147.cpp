  static void  Ins_MD( INS_ARG )
  {
    Long       K, L;
    TT_F26Dot6 D;

    K = args[1];
    L = args[0];

    if( BOUNDS( args[0], CUR.zp2.n_points ) ||
        BOUNDS( args[1], CUR.zp1.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( CUR.opcode & 1 )
      D = CUR_Func_project( CUR.zp2.cur_x[L] - CUR.zp1.cur_x[K],
                            CUR.zp2.cur_y[L] - CUR.zp1.cur_y[K] );
    else
      D = CUR_Func_dualproj( CUR.zp2.org_x[L] - CUR.zp1.org_x[K],
                             CUR.zp2.org_y[L] - CUR.zp1.org_y[K] );

    args[0] = D;
  }