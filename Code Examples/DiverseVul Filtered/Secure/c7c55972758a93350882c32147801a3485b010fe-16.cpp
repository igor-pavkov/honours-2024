  static void  Ins_SZP1( INS_ARG )
  {
    switch ( args[0] )
    {
    case 0:
      CUR.zp1 = CUR.twilight;
      break;

    case 1:
      CUR.zp1 = CUR.pts;
      break;

    default:
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.GS.gep1 = (Int)(args[0]);
  }