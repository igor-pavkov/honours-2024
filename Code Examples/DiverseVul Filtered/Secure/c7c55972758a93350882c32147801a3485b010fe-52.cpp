  static void  Write_CVT( EXEC_OPS Int  index, TT_F26Dot6  value )
  {
    int ov=CUR.cvt[index];
    (void)ov; /* Quiet compiler warning in release build. */
    CUR.cvt[index] = value;
    DBG_PRINT3(" cvt[%d]%d:=%d", index, ov, CUR.cvt[index]);
}