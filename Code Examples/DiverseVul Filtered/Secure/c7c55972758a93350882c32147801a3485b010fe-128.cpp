  static void  Ins_UTP( INS_ARG )
  {
    Byte  mask;

    if ( BOUNDS( args[0], CUR.zp0.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    mask = 0xFF;

    if ( CUR.GS.freeVector.x != 0 )
      mask &= ~TT_Flag_Touched_X;

    if ( CUR.GS.freeVector.y != 0 )
      mask &= ~TT_Flag_Touched_Y;

    CUR.zp0.touch[args[0]] &= mask;
  }