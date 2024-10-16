  static Bool  Ins_SxVTL( EXEC_OPS  Int             aIdx1,
                                    Int             aIdx2,
                                    Int             aOpc,
                                    TT_UnitVector*  Vec )
  {
    Long  A, B, C;

    if ( BOUNDS( aIdx1, CUR.zp2.n_points ) ||
         BOUNDS( aIdx2, CUR.zp1.n_points ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return FAILURE;
    }

    A = CUR.zp1.cur_x[aIdx2] - CUR.zp2.cur_x[aIdx1];
    B = CUR.zp1.cur_y[aIdx2] - CUR.zp2.cur_y[aIdx1];

    if ( (aOpc & 1) != 0 )
    {
      C =  B;   /* CounterClockwise rotation */
      B =  A;
      A = -C;
    }

    if ( NORMalize( A, B, Vec ) == FAILURE )
    {
      /* When the vector is too small or zero! */

      CUR.error = TT_Err_Ok;
      Vec->x = 0x4000;
      Vec->y = 0;
    }

    return SUCCESS;
  }