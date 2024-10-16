  static void  Ins_SCANTYPE( INS_ARG )
  {
    /* For compatibility with future enhancements, */
    /* we must ignore new modes                    */

    if ( args[0] >= 0 && args[0] <= 5 )
    {
      if ( args[0] == 3 )
        args[0] = 2;

      CUR.GS.scan_type = (Int)args[0];
    }
  }