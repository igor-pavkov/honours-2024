  static void  Move_Zp2_Point( EXEC_OPS
                               Long        point,
                               TT_F26Dot6  dx,
                               TT_F26Dot6  dy,
                               Bool        touch )
  {
    if ( CUR.GS.freeVector.x != 0 )
    {
      CUR.zp2.cur_x[point] += dx;
      if ( touch )
        CUR.zp2.touch[point] |= TT_Flag_Touched_X;
    }

    if ( CUR.GS.freeVector.y != 0 )
    {
      CUR.zp2.cur_y[point] += dy;
      if ( touch )
        CUR.zp2.touch[point] |= TT_Flag_Touched_Y;
    }
  }