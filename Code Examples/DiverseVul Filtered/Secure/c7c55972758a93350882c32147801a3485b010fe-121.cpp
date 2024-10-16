  static TT_F26Dot6  Round_To_Grid( EXEC_OPS TT_F26Dot6  distance,
                                             TT_F26Dot6  compensation )
  {
    TT_F26Dot6  val;
    (void)exc;

    if ( distance >= 0 )
    {
      val = (distance + compensation + 32) & (-64);
      if ( val < 0 )
        val = 0;
    }
    else
    {
      val = -( (compensation - distance + 32) & (-64) );
      if ( val > 0 )
        val = 0;
    }

    return  val;
  }