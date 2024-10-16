  static void  Ins_JMPR( INS_ARG )
  {
    if ( BOUNDS(CUR.IP + args[0], CUR.codeSize ) )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.IP      += (Int)(args[0]);
    CUR.step_ins = FALSE;

    if(CUR.IP > CUR.codeSize ||
       (CUR.code[CUR.IP] != 0x2D && CUR.code[CUR.IP - 1] == 0x2D))
    /* The JPMR is meant to stop at the ENDF instruction to finish
     * the function. However the programmer made a mistake, and ended
     * up one byte too far. I suspect that some TT interpreters handle this
     * by detecting that the IP has gone off the end of the function. We can
     * allow for simple cases here by just checking the preceding byte.
     * Fonts with this problem are not uncommon.
     */
      CUR.IP -= 1;
  }