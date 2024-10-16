  static void  Ins_DELTAC( INS_ARG )
  {
    Long  nump, k;
    Long  A, B, C;

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

      if ( A >= CUR.cvtSize )
      {
        CUR.error = TT_Err_Invalid_Reference;
        return;
      }

      C = ((unsigned long)(B & 0xF0)) >> 4;

      switch ( CUR.opcode )
      {
      case 0x73:
        break;

      case 0x74:
        C += 16;
        break;

      case 0x75:
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

        CUR_Func_move_cvt( A, B );
      }
    }

    CUR.new_top = CUR.args;
  }