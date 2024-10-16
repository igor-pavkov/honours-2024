  static Bool  SkipCode( EXEC_OP )
  {
    CUR.IP += CUR.length;

    if ( CUR.IP < CUR.codeSize )
      if ( CALC_Length() == SUCCESS )
        return SUCCESS;

    CUR.error = TT_Err_Code_Overflow;
    return FAILURE;
  }