  static Long  Current_Ratio( EXEC_OP )
  {
    if ( CUR.metrics.ratio )
      return CUR.metrics.ratio;

    if ( CUR.GS.projVector.y == 0 )
      CUR.metrics.ratio = CUR.metrics.x_ratio;

    else if ( CUR.GS.projVector.x == 0 )
      CUR.metrics.ratio = CUR.metrics.y_ratio;

    else
    {
      Long  x, y;

      x = MulDiv_Round( CUR.GS.projVector.x, CUR.metrics.x_ratio, 0x4000 );
      y = MulDiv_Round( CUR.GS.projVector.y, CUR.metrics.y_ratio, 0x4000 );
      CUR.metrics.ratio = Norm( x, y );
    }

    return CUR.metrics.ratio;
  }