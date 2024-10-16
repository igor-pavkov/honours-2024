  static void  Compute_Round( EXEC_OPS Byte  round_mode )
  {
    switch ( round_mode )
    {
    case TT_Round_Off:
      CUR.func_round = (TRound_Function)Round_None;
      break;

    case TT_Round_To_Grid:
      CUR.func_round = (TRound_Function)Round_To_Grid;
      break;

    case TT_Round_Up_To_Grid:
      CUR.func_round = (TRound_Function)Round_Up_To_Grid;
      break;

    case TT_Round_Down_To_Grid:
      CUR.func_round = (TRound_Function)Round_Down_To_Grid;
      break;

    case TT_Round_To_Half_Grid:
      CUR.func_round = (TRound_Function)Round_To_Half_Grid;
      break;

    case TT_Round_To_Double_Grid:
      CUR.func_round = (TRound_Function)Round_To_Double_Grid;
      break;

    case TT_Round_Super:
      CUR.func_round = (TRound_Function)Round_Super;
      break;

    case TT_Round_Super_45:
      CUR.func_round = (TRound_Function)Round_Super_45;
      break;
    }
  }