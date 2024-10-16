  static void  Ins_SCANCTRL( INS_ARG )
  {
    Int  A;

    /* Get Threshold */
    A = (Int)(args[0] & 0xFF);

    if ( A == 0xFF )
    {
      CUR.GS.scan_control = TRUE;
      return;
    }
    else if ( A == 0 )
    {
      CUR.GS.scan_control = FALSE;
      return;
    }

    A *= 64;

    if ( (args[0] & 0x100) != 0 && CUR.metrics.pointSize <= A )
      CUR.GS.scan_control = TRUE;

    if ( (args[0] & 0x200) != 0 && CUR.metrics.rotated )
      CUR.GS.scan_control = TRUE;

    if ( (args[0] & 0x400) != 0 && CUR.metrics.stretched )
      CUR.GS.scan_control = TRUE;

    if ( (args[0] & 0x800) != 0 && CUR.metrics.pointSize > A )
      CUR.GS.scan_control = FALSE;

    if ( (args[0] & 0x1000) != 0 && CUR.metrics.rotated )
      CUR.GS.scan_control = FALSE;

    if ( (args[0] & 0x2000) != 0 && CUR.metrics.stretched )
      CUR.GS.scan_control = FALSE;
}