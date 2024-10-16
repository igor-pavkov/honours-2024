  static void  Ins_CALL( INS_ARG )
  {
    PCallRecord  pCrec;

    if ( BOUNDS( args[0], CUR.numFDefs ) || !CUR.FDefs[args[0]].Active )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( CUR.callTop >= CUR.callSize )
    {
      CUR.error = TT_Err_Stack_Overflow;
      return;
    }

    DBG_PRINT1("%d", args[0]);

    pCrec = &CUR.callStack[CUR.callTop];

    pCrec->Caller_Range = CUR.curRange;
    pCrec->Caller_IP    = CUR.IP + 1;
    pCrec->Cur_Count    = 1;
    pCrec->Cur_Restart  = CUR.FDefs[args[0]].Start;

    CUR.callTop++;

    INS_Goto_CodeRange( CUR.FDefs[args[0]].Range,
                        CUR.FDefs[args[0]].Start );

    CUR.step_ins = FALSE;
  }