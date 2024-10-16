  static void  Ins_UNKNOWN( INS_ARG )
  { /* Rewritten by igorm. */
    Byte i;
    TDefRecord*  def;
    PCallRecord  call;

#   if 0     /* The condition below appears always false
                due to limited range of data type
                - skip it to quiet a compiler warning. */
    if (CUR.opcode > sizeof(CUR.IDefPtr) / sizeof(CUR.IDefPtr[0])) {
        CUR.error = TT_Err_Invalid_Opcode;
        return;
    }
#   endif
    i = CUR.IDefPtr[(Byte)CUR.opcode];

    if (i >= CUR.numIDefs)
      {
        CUR.error = TT_Err_Invalid_Opcode;
        return;
      }
    def   = &CUR.IDefs[i];

    if ( CUR.callTop >= CUR.callSize )
    {
      CUR.error = TT_Err_Stack_Overflow;
      return;
    }

    call = CUR.callStack + CUR.callTop++;

    call->Caller_Range = CUR.curRange;
    call->Caller_IP    = CUR.IP+1;
    call->Cur_Count    = 1;
    call->Cur_Restart  = def->Start;

    INS_Goto_CodeRange( def->Range, def->Start );

    CUR.step_ins = FALSE;
    return;
  }