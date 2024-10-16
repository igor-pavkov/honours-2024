  static void  Ins_RTG( INS_ARG )
  { (void)args;
    CUR.GS.round_state = TT_Round_To_Grid;

    CUR.func_round = (TRound_Function)Round_To_Grid;
  }