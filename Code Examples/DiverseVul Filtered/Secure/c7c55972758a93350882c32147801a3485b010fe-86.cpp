  static void  Ins_SHZ( INS_ARG )
  {
    TGlyph_Zone zp;
    Int         refp;
    TT_F26Dot6  dx,
                dy;

    Int  last_point;
    Long i;

    if ( BOUNDS( args[0], 2 ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    last_point = zp.n_points - 1;

    /* undocumented: SHZ doesn't touch the points */
    for ( i = 0; i <= last_point; i++ )
    {
      if ( zp.cur_x != CUR.zp2.cur_x || refp != i )
        MOVE_Zp2_Point( i, dx, dy, FALSE );
    }
  }