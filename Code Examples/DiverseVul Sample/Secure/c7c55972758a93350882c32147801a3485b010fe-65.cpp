  static void  Ins_FDEF( INS_ARG )
  {
    PDefRecord  pRec;

    if ( BOUNDS( args[0], CUR.numFDefs ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    pRec = &CUR.FDefs[args[0]];

    pRec->Range  = CUR.curRange;
    pRec->Opc    = (Byte)(args[0]);
    pRec->Start  = CUR.IP + 1;
    pRec->Active = TRUE;

    skip_FDEF(EXEC_ARG);
  }