  static void  Ins_RDTG( INS_ARG )
  { (void)args;
    CUR.GS.round_state = TT_Round_Down_To_Grid;

    CUR.func_round = (TRound_Function)Round_Down_To_Grid;
  }