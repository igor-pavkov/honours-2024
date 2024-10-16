  static void  Ins_SHPIX( INS_ARG )
  {
    TT_F26Dot6  dx, dy;
    Long        point;

    if ( CUR.top < CUR.GS.loop )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    dx = MulDiv_Round( args[0],
                       (Long)CUR.GS.freeVector.x,
                       0x4000 );
    dy = MulDiv_Round( args[0],
                       (Long)CUR.GS.freeVector.y,
                       0x4000 );

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;

      point = CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp2.n_points ) )
      {
        CUR.error = TT_Err_Invalid_Reference;
        return;
      }

      MOVE_Zp2_Point( point, dx, dy, TRUE );

      CUR.GS.loop--;
    }

    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }