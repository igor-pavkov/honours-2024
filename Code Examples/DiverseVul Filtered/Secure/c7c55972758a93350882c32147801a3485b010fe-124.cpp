  static void  Ins_ALIGNRP( INS_ARG )
  {
    Int         point;
    TT_F26Dot6  distance;
    (void)args;

    if ( CUR.top < CUR.GS.loop )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = (Int)CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp1.n_points ) )
      {
        CUR.error = TT_Err_Invalid_Reference;
        return;
      }

      distance = CUR_Func_project( CUR.zp1.cur_x[point] -
                                     CUR.zp0.cur_x[CUR.GS.rp0],
                                   CUR.zp1.cur_y[point] -
                                     CUR.zp0.cur_y[CUR.GS.rp0] );

      CUR_Func_move( &CUR.zp1, point, -distance );
      CUR.GS.loop--;
    }

    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }