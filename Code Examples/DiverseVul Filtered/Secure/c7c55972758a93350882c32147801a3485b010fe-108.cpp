  static void  Ins_GETINFO( INS_ARG )
  {
    Long  K;

    K = 0;

    /* We return then Windows 3.1 version number */
    /* for the font scaler                       */
    if ( (args[0] & 1) != 0 )
      K = 3;

    /* Has the glyph been rotated ? */
    if ( CUR.metrics.rotated )
      K |= 0x80;

    /* Has the glyph been stretched ? */
    if ( CUR.metrics.stretched )
      K |= 0x100;

    args[0] = K;
  }