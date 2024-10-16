  static void  Ins_SROUND( INS_ARG )
  {
    SET_SuperRound( 0x4000L, args[0] );
    CUR.GS.round_state = TT_Round_Super;

    CUR.func_round = (TRound_Function)Round_Super;
  }