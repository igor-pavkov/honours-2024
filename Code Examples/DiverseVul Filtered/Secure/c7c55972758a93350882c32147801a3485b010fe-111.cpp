  static void Ins_IDEF( INS_ARG )
  {
    if (CUR.countIDefs >= CUR.numIDefs || args[0] > 255)
        CUR.error = TT_Err_Storage_Overflow;
    else
      {
        PDefRecord  pTDR;

        CUR.IDefPtr[(Byte)(args[0])] = CUR.countIDefs;
        pTDR = &CUR.IDefs[CUR.countIDefs++];
        pTDR->Opc    = (Byte)(args[0]);
        pTDR->Start  = CUR.IP + 1;
        pTDR->Range  = CUR.curRange;
        pTDR->Active = TRUE;
        skip_FDEF(EXEC_ARG);
      }
  }