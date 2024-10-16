  static Bool  Compute_Point_Displacement( EXEC_OPS
                                           PCoordinates  x,
                                           PCoordinates  y,
                                           PGlyph_Zone   zone,
                                           Int*          refp )
  {
    TGlyph_Zone  zp;
    Int          p;
    TT_F26Dot6   d;

    if ( CUR.opcode & 1 )
    {
      zp = CUR.zp0;
      p  = CUR.GS.rp1;
    }
    else
    {
      zp = CUR.zp1;
      p  = CUR.GS.rp2;
    }

    if ( BOUNDS( p, zp.n_points ) )
    {
      /* Don't set error code, just return. */
      /* Ported from Freetype2 */
      *refp = 0;
      return FAILURE;
    }

    *zone = zp;
    *refp = p;

    d = CUR_Func_project( zp.cur_x[p] - zp.org_x[p],
                          zp.cur_y[p] - zp.org_y[p] );

    *x = MulDiv_Round(d, (Long)CUR.GS.freeVector.x * 0x10000L, CUR.F_dot_P );
    *y = MulDiv_Round(d, (Long)CUR.GS.freeVector.y * 0x10000L, CUR.F_dot_P );

    return SUCCESS;
  }