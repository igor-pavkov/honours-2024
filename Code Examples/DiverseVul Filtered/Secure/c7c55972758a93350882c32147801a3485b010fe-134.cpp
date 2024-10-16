  static void  Ins_LOOPCALL( INS_ARG )
  {
    PCallRecord  pTCR;

    if ( BOUNDS( args[1], CUR.numFDefs ) || !CUR.FDefs[args[1]].Active )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( CUR.callTop >= CUR.callSize )
    {
      CUR.error = TT_Err_Stack_Overflow;
      return;
    }

    if ( args[0] > 0 )
    {
      pTCR = &CUR.callStack[CUR.callTop];

      pTCR->Caller_Range = CUR.curRange;
      pTCR->Caller_IP    = CUR.IP + 1;
      pTCR->Cur_Count    = (Int)(args[0]);
      pTCR->Cur_Restart  = CUR.FDefs[args[1]].Start;

      CUR.callTop++;

      INS_Goto_CodeRange( CUR.FDefs[args[1]].Range,
                          CUR.FDefs[args[1]].Start );

      CUR.step_ins = FALSE;
    }
  }