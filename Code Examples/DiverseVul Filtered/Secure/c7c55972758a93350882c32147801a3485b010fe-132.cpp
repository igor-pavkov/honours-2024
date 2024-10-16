  static void  Ins_SPVFS( INS_ARG )
  {
    Short  S;
    Long   X, Y;

    /* Only use low 16bits, then sign extend */
    S = (Short)args[1];
    Y = (Long)S;
    S = (Short)args[0];
    X = (Long)S;

    if ( NORMalize( X, Y, &CUR.GS.projVector ) == FAILURE )
      return;

    CUR.GS.dualVector = CUR.GS.projVector;

    COMPUTE_Funcs();
  }