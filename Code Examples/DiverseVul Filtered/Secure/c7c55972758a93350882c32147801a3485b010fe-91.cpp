  static void skip_FDEF( EXEC_OP )
  {
    /* We don't allow nested IDEFS & FDEFs.    */

    while ( SKIP_Code() == SUCCESS )
    {
      switch ( CUR.opcode )
      {
      case 0x89:    /* IDEF */
      case 0x2c:    /* FDEF */
        CUR.error = TT_Err_Nested_DEFS;
        return;

      case 0x2d:   /* ENDF */
        return;
      }
    }
  }