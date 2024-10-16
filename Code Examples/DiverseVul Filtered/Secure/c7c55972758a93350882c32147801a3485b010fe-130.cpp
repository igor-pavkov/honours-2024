  static void  Ins_SHC( INS_ARG )
  {
    TGlyph_Zone zp;
    Int         refp;
    TT_F26Dot6  dx,
                dy;

    Long        contour, i;
    Int         first_point, last_point;

    contour = args[0];

    if ( BOUNDS( args[0], CUR.pts.n_contours ) )
    {
#if 0
      /* A workaround for the Ghostscript bug 688501.
       *  Ported from FreeType 2
       */
      CUR.error = TT_Err_Invalid_Reference;
#endif
      return;
    }

    if ( COMPUTE_Point_Displacement( &dx, &dy, &zp, &refp ) )
      return;

    if ( contour == 0 )
      first_point = 0;
    else
      first_point = CUR.pts.contours[contour-1] + 1;

    last_point = CUR.pts.contours[contour];

    /* undocumented: SHC doesn't touch the points */
    for ( i = first_point; i <= last_point; i++ )
    {
      if ( zp.cur_x != CUR.zp2.cur_x || refp != i )
        MOVE_Zp2_Point( i, dx, dy, FALSE );
    }
  }