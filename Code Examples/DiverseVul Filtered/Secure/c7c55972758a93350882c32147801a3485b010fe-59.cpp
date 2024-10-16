  static void  Ins_RUTG( INS_ARG )
  { (void)args;
    CUR.GS.round_state = TT_Round_Up_To_Grid;

    CUR.func_round = (TRound_Function)Round_Up_To_Grid;
  }