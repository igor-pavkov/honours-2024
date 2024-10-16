  static void  Ins_MDAP( INS_ARG )
  {
    Int         point;
    TT_F26Dot6  cur_dist,
                distance;

    point = (Int)args[0];

    if ( BOUNDS( args[0], CUR.zp0.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    /* XXX: Is there some undocumented feature while in the */
    /*      twilight zone? ?                                */

    if ( (CUR.opcode & 1) != 0 )
    {
      cur_dist = CUR_Func_project( CUR.zp0.cur_x[point],
                                   CUR.zp0.cur_y[point] );
      distance = CUR_Func_round( cur_dist,
                                 CUR.metrics.compensations[0] ) - cur_dist;
    }
    else
      distance = 0;

    CUR_Func_move( &CUR.zp0, point, distance );

    CUR.GS.rp0 = point;
    CUR.GS.rp1 = point;
  }