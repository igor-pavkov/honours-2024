  static void  Direct_Move( EXEC_OPS PGlyph_Zone zone,
                                     Int         point,
                                     TT_F26Dot6  distance )
  {
    TT_F26Dot6 v;

    v = CUR.GS.freeVector.x;

    if ( v != 0 )
    {
      zone->cur_x[point] += MulDiv_Round( distance,
                                          v * 0x10000L,
                                          CUR.F_dot_P );

      zone->touch[point] |= TT_Flag_Touched_X;
    }

    v = CUR.GS.freeVector.y;

    if ( v != 0 )
    {
      zone->cur_y[point] += MulDiv_Round( distance,
                                          v * 0x10000L,
                                          CUR.F_dot_P );

      zone->touch[point] |= TT_Flag_Touched_Y;
    }
  }