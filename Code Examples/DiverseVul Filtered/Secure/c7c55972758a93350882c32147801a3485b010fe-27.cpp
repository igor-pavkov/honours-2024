  static TT_F26Dot6  Read_CVT_Stretched( EXEC_OPS Int  index )
  {
    return MulDiv_Round( CUR.cvt[index], CURRENT_Ratio(), 0x10000 );
  }