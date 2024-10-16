  static void  Ins_S45ROUND( INS_ARG )
  {
    SET_SuperRound( 0x2D41L, args[0] );
    CUR.GS.round_state = TT_Round_Super_45;

    CUR.func_round = (TRound_Function)Round_Super_45;
  }