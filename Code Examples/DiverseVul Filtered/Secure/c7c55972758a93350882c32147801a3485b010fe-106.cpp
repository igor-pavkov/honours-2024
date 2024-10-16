  static void  Ins_RTHG( INS_ARG )
  { (void)args;
    CUR.GS.round_state = TT_Round_To_Half_Grid;

    CUR.func_round = (TRound_Function)Round_To_Half_Grid;
  }