  static void  Ins_SFVFS( INS_ARG )
  {
    Short  S;
    Long   X, Y;

    /* Only use low 16bits, then sign extend */
    S = (Short)args[1];
    Y = (Long)S;
    S = (Short)args[0];
    X = S;

    if ( NORMalize( X, Y, &CUR.GS.freeVector ) == FAILURE )
      return;

    COMPUTE_Funcs();
  }