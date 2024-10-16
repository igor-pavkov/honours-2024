  static void  Ins_ENDF( INS_ARG )
  {
    PCallRecord  pRec;
     (void)args;

    if ( CUR.callTop <= 0 )     /* We encountered an ENDF without a call */
    {
      CUR.error = TT_Err_ENDF_In_Exec_Stream;
      return;
    }

    CUR.callTop--;

    pRec = &CUR.callStack[CUR.callTop];

    pRec->Cur_Count--;

    CUR.step_ins = FALSE;

    if ( pRec->Cur_Count > 0 )
    {
      CUR.callTop++;
      CUR.IP = pRec->Cur_Restart;
    }
    else
      /* Loop through the current function */
      INS_Goto_CodeRange( pRec->Caller_Range,
                          pRec->Caller_IP );

    /* Exit the current call frame.                       */

    /* NOTE: When the last intruction of a program        */
    /*       is a CALL or LOOPCALL, the return address    */
    /*       is always out of the code range.  This is    */
    /*       a valid address, and it's why we do not test */
    /*       the result of Ins_Goto_CodeRange() here!     */
  }