  static void  Ins_ROFF( INS_ARG )
  { (void)args;
    CUR.GS.round_state = TT_Round_Off;

    CUR.func_round = (TRound_Function)Round_None;
  }