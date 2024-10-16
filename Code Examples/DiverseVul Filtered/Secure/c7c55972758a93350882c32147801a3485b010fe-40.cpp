  static void  Ins_JROF( INS_ARG )
  {
    if ( args[1] == 0 )
    {
      CUR.IP      += (Int)(args[0]);
      CUR.step_ins = FALSE;

      /* See JMPR above */
      if(CUR.code[CUR.IP] != 0x2D && CUR.code[CUR.IP - 1] == 0x2D)
        CUR.IP -= 1;
    }
  }