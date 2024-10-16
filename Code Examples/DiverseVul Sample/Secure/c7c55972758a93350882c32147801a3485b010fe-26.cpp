  static Short  GetShortIns( EXEC_OP )
  {
    /* Reading a byte stream so there is no endianess (DaveP) */
    CUR.IP += 2;
    return ( CUR.code[CUR.IP-2] << 8) +
             CUR.code[CUR.IP-1];
  }