  static void  Ins_FLIPPT( INS_ARG )
  {
    Long  point;
    (void)args;

    if ( CUR.top < CUR.GS.loop )
    {
      CUR.error = TT_Err_Too_Few_Arguments;
      return;
    }

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.pts.n_points ) )
      {
        CUR.error = TT_Err_Invalid_Reference;
       return;
      }

      CUR.pts.touch[point] ^= TT_Flag_On_Curve;

      CUR.GS.loop--;
    }

    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }