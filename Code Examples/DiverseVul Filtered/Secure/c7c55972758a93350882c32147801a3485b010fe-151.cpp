  static TT_F26Dot6  Round_To_Double_Grid( EXEC_OPS TT_F26Dot6  distance,
                                                    TT_F26Dot6  compensation )
  {
    TT_F26Dot6 val;
    (void)exc;

    if ( distance >= 0 )
    {
      val = (distance + compensation + 16) & (-32);
      if ( val < 0 )
        val = 0;
    }
    else
    {
      val = -( (compensation - distance + 16) & (-32) );
      if ( val > 0 )
        val = 0;
    }

    return val;
  }