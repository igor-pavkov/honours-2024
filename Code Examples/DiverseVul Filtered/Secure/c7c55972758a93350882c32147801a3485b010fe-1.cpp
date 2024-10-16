  static void  Ins_SZP0( INS_ARG )
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
      break;
    }

    CUR.GS.gep0 = (Int)(args[0]);
  }