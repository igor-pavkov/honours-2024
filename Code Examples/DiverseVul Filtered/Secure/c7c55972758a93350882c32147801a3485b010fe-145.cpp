  static void  Ins_SHP( INS_ARG )
  {
    TGlyph_Zone zp;
    Int         refp;

    TT_F26Dot6  dx,
                dy;
    Long        point;
    (void)args;

    if ( CUR.top < CUR.GS.loop )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    while ( CUR.GS.loop > 0 )
    {
      CUR.args--;
      point = CUR.stack[CUR.args];

      if ( BOUNDS( point, CUR.zp2.n_points ) )
      {
        CUR.error = TT_Err_Invalid_Reference;
        return;
      }

      /* undocumented: SHP touches the points */
      MOVE_Zp2_Point( point, dx, dy, TRUE );

      CUR.GS.loop--;
    }

    CUR.GS.loop = 1;
    CUR.new_top = CUR.args;
  }