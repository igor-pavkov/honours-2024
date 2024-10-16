  static void  Ins_SZPS( INS_ARG )
  {
    switch ( args[0] )
    {
    case 0:
      CUR.zp0 = CUR.twilight;
      break;

    case 1:
      CUR.zp0 = CUR.pts;
      break;

    default:
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.zp1 = CUR.zp0;
    CUR.zp2 = CUR.zp0;

    CUR.GS.gep0 = (Int)(args[0]);
    CUR.GS.gep1 = (Int)(args[0]);
    CUR.GS.gep2 = (Int)(args[0]);
  }