  static TT_F26Dot6  FUnits_To_Pixels( EXEC_OPS  Int  distance )
  {
    return MulDiv_Round( distance,
                         CUR.metrics.scale1,
                         CUR.metrics.scale2 );
  }