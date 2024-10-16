  static void  Ins_ALIGNPTS( INS_ARG )
  {
    Int         p1, p2;
    TT_F26Dot6  distance;

    p1 = (Int)args[0];
   p2 = (Int)args[1];

    if ( BOUNDS( args[0], CUR.zp1.n_points ) ||
         BOUNDS( args[1], CUR.zp0.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    distance = CUR_Func_project( CUR.zp0.cur_x[p2] -
                                   CUR.zp1.cur_x[p1],
                                 CUR.zp0.cur_y[p2] -
                                   CUR.zp1.cur_y[p1] ) / 2;

    CUR_Func_move( &CUR.zp1, p1, distance );

    CUR_Func_move( &CUR.zp0, p2, -distance );
  }