  static TT_F26Dot6  Norm( TT_F26Dot6  X, TT_F26Dot6  Y )
  {
    Int64       T1, T2;

    MUL_64( X, X, T1 );
    MUL_64( Y, Y, T2 );

    ADD_64( T1, T2, T1 );

    return (TT_F26Dot6)SQRT_64( T1 );
  }