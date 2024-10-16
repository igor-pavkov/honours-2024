  static void  Ins_INSTCTRL( INS_ARG )
  {
    Long  K, L;

    K = args[1];
    L = args[0];

    if ( K < 0 || K > 3 )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.GS.instruct_control = (Int)((CUR.GS.instruct_control & (~K)) | (L & K));
  }