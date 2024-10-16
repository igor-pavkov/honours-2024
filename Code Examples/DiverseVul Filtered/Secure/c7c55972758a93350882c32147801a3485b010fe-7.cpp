  static void  Ins_FLIPRGON( INS_ARG )
  {
    Long  I, K, L;

    K = args[1];
    L = args[0];

    if ( BOUNDS( K, CUR.pts.n_points ) ||
         BOUNDS( L, CUR.pts.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    for ( I = L; I <= K; I++ )
      CUR.pts.touch[I] |= TT_Flag_On_Curve;
  }