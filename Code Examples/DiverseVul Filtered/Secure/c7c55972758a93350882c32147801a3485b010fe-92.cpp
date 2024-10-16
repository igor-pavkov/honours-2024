  static void  Direct_Move_Y( EXEC_OPS PGlyph_Zone  zone,
                                       Int         point,
                                       TT_F26Dot6  distance )
  { (void)exc;
    zone->cur_y[point] += distance;
    zone->touch[point] |= TT_Flag_Touched_Y;
  }