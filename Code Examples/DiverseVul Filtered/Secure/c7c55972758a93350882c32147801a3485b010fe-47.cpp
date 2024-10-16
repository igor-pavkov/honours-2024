  static void  Ins_DELTAP( INS_ARG )
  {
    Int   k;
    Long  A, B, C, nump;

    nump = args[0];

    for ( k = 1; k <= nump; k++ )
    {
      if ( CUR.args < 2 )
      {
        CUR.error = TT_Err_Too_Few_Arguments;
        return;
      }

      CUR.args -= 2;

      A = CUR.stack[CUR.args + 1];
      B = CUR.stack[CUR.args];

#if 0
      if ( BOUNDS( A, CUR.zp0.n_points ) )
#else
      /* igorm changed : allow phantom points (Altona.Page_3.2002-09-27.pdf). */
      if ( BOUNDS( A, CUR.zp0.n_points + 2 ) )
#endif
      {
/*        CUR.error = TT_Err_Invalid_Reference;*/
        return;
      }

      C = (B & 0xF0) >> 4;

      switch ( CUR.opcode )
      {
      case 0x5d:
        break;

      case 0x71:
        C += 16;
        break;

      case 0x72:
        C += 32;
        break;
      }

      C += CUR.GS.delta_base;

      if ( CURRENT_Ppem() == C )
      {
        B = (B & 0xF) - 8;
        if ( B >= 0 )
          B++;
        B = B * 64 / (1L << CUR.GS.delta_shift);

        CUR_Func_move( &CUR.zp0, (Int)A, (Int)B );
      }
    }

    CUR.new_top = CUR.args;
  }