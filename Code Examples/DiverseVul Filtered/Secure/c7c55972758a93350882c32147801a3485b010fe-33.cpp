  static void  Ins_NPUSHW( INS_ARG )
  {
    Int  L, K;

    L = (Int)CUR.code[CUR.IP + 1];

    if ( BOUNDS( L, CUR.stackSize+1-CUR.top ) )
    {
      CUR.error = TT_Err_Stack_Overflow;
      return;
    }

    CUR.IP += 2;

    for ( K = 0; K < L; K++ )
      { args[K] = GET_ShortIns();
        DBG_PRINT1(" %d", args[K]);
      }

    CUR.step_ins = FALSE;
    CUR.new_top += L;
  }