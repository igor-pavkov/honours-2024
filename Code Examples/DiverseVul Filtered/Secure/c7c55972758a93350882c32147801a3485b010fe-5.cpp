  static void  Ins_SSW( INS_ARG )
  {
    /* XXX : Undocumented or bug in the Windows engine ?  */
    /*                                                    */
    /* It seems that the value that is read here is       */
    /* expressed in 16.16 format, rather than in          */
    /* font units..                                       */
    /*                                                    */
    CUR.GS.single_width_value = (TT_F26Dot6)(args[0] >> 10);
  }