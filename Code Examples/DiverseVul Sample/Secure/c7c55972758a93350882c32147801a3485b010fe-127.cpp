  static void  Ins_GC( INS_ARG )
  {
    Long  L;

    L = args[0];

    if ( BOUNDS( L, CUR.zp2.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    switch ( CUR.opcode & 1 )
    {
    case 0:
      L = CUR_Func_project( CUR.zp2.cur_x[L],
                            CUR.zp2.cur_y[L] );
      break;

    case 1:
      L = CUR_Func_dualproj( CUR.zp2.org_x[L],
                             CUR.zp2.org_y[L] );
      break;
    }

    args[0] = L;
  }