  static TT_F26Dot6  Round_Super( EXEC_OPS TT_F26Dot6  distance,
                                           TT_F26Dot6  compensation )
  {
    TT_F26Dot6  val;

    if ( distance >= 0 )
    {
      val = (distance - CUR.phase + CUR.threshold + compensation) &
              (-CUR.period);
      if ( val < 0 )
        val = 0;
      val += CUR.phase;
    }
    else
    {
      val = -( (CUR.threshold - CUR.phase - distance + compensation) &
               (-CUR.period) );
      if ( val > 0 )
        val = 0;
      val -= CUR.phase;
    }

    return val;
  }