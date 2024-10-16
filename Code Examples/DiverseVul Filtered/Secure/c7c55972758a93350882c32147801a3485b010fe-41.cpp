  TT_Error  RunIns( PExecution_Context  exc )
  {
    TT_Error     Result;
    Int          A;
    PDefRecord   WITH;
    PCallRecord  WITH1;
#if defined(DEBUG) && !defined(GS_THREADSAFE)
    bool bFirst;
#endif
    bool dbg_prt = (DBG_PRT_FUN != NULL);
#   ifdef DEBUG
        ttfMemory *mem = exc->current_face->font->tti->ttf_memory;
        F26Dot6 *save_ox, *save_oy, *save_cx, *save_cy;

        DBG_PRINT("\n%% *** Entering RunIns ***");
#   endif

    (void)dbg_prt; /* Quiet compiler warning in release build. */

    /* set CVT functions */
    CUR.metrics.ratio = 0;
    if ( CUR.metrics.x_ppem != CUR.metrics.y_ppem )
    {
      /* non-square pixels, use the stretched routines */
      CUR.func_read_cvt  = Read_CVT_Stretched;
      CUR.func_write_cvt = Write_CVT_Stretched;
      CUR.func_move_cvt  = Move_CVT_Stretched;
    }
    else
    {
      /* square pixels, use normal routines */
      CUR.func_read_cvt  = Read_CVT;
      CUR.func_write_cvt = Write_CVT;
      CUR.func_move_cvt  = Move_CVT;
    }

    COMPUTE_Funcs();
    Compute_Round( EXEC_ARGS (Byte)exc->GS.round_state );

#   ifdef DEBUG
      if (dbg_prt && CUR.pts.n_points) {
        save_ox = mem->alloc_bytes(mem, CUR.pts.n_points * sizeof(*save_ox), "RunIns");
        save_oy = mem->alloc_bytes(mem, CUR.pts.n_points * sizeof(*save_oy), "RunIns");
        save_cx = mem->alloc_bytes(mem, CUR.pts.n_points * sizeof(*save_cx), "RunIns");
        save_cy = mem->alloc_bytes(mem, CUR.pts.n_points * sizeof(*save_cy), "RunIns");
        if (!save_ox || !save_oy || !save_cx || !save_cy)
          return TT_Err_Out_Of_Memory;
      } else
        save_ox = save_oy = save_cx = save_cy = NULL;
#   endif

    Result = setjmp(find_jmp_buf(exc->trap));
    if (Result) {
        CUR.error = Result;
        goto _LExit;
    }
#if defined(DEBUG) && !defined(GS_THREADSAFE)
    bFirst = true;
#endif
    do
    {
      CALC_Length();

      /* First, let's check for empty stack and overflow */

      CUR.args = CUR.top - Pop_Push_Count[CUR.opcode * 2];

      /* `args' is the top of the stack once arguments have been popped. */
      /* One can also interpret it as the index of the last argument.    */

      if ( CUR.args < 0 )
      {
        CUR.error = TT_Err_Too_Few_Arguments;
        goto _LErrorLabel;
      }

      CUR.new_top = CUR.args + Pop_Push_Count[CUR.opcode * 2 + 1];

      /* `new_top' is the new top of the stack, after the instruction's */
      /* execution.  `top' will be set to `new_top' after the 'switch'  */
      /* statement.                                                     */

      if ( CUR.new_top > CUR.stackSize )
      {
        CUR.error = TT_Err_Stack_Overflow;
        goto _LErrorLabel;
      }

      CUR.step_ins = TRUE;
      CUR.error    = TT_Err_Ok;

#     if defined(DEBUG) && !defined(GS_THREADSAFE)
        DBG_PRINT3("\n%%n=%5d IP=%5d OP=%s            ", nInstrCount, CUR.IP, Instruct_Dispatch[CUR.opcode].sName);
        /*
        { for(int i=0;i<CUR.top;i++)
            DBG_PRINT1("% %d",CUR.stack[i]);
        }
        */
        if (save_ox != NULL) {
          memcpy(save_ox, CUR.pts.org_x, sizeof(CUR.pts.org_x[0]) * CUR.pts.n_points);
          memcpy(save_oy, CUR.pts.org_y, sizeof(CUR.pts.org_y[0]) * CUR.pts.n_points);
          memcpy(save_cx, CUR.pts.cur_x, sizeof(CUR.pts.cur_x[0]) * CUR.pts.n_points);
          memcpy(save_cy, CUR.pts.cur_y, sizeof(CUR.pts.cur_y[0]) * CUR.pts.n_points);
        }
#     endif

      Instruct_Dispatch[CUR.opcode].p( EXEC_ARGS &CUR.stack[CUR.args] );

#     if defined(DEBUG) && !defined(GS_THREADSAFE)
      if (save_ox != NULL) {
        F26Dot6 *pp[4], *qq[4];
        const char *ss[] = {"org.x", "org.y", "cur.x", "cur.y"};
        int l = 0, i, j;

        pp[0] = save_ox,
        pp[1] = save_oy,
        pp[2] = save_cx,
        pp[3] = save_cy;
        qq[0] = CUR.pts.org_x;
        qq[1] = CUR.pts.org_y;
        qq[2] = CUR.pts.cur_x;
        qq[3] = CUR.pts.cur_y;

        for(i = 0; i < 4; i++)
          for(j = 0;j < CUR.pts.n_points; j++)
            { F26Dot6 *ppi = pp[i], *qqi = qq[i];
              if(ppi[j] != qqi[j] || bFirst)
              {
                DBG_PRINT4("%%  %s[%d]%d:=%d", ss[i], j, pp[i][j], qq[i][j]);
                if(++l > 3)
                  { l=0;
                    DBG_PRINT("\n");
                  }
              }
            }
        nInstrCount++;
        bFirst=FALSE;
      }
#     endif

      DBG_PAINT

      if ( CUR.error != TT_Err_Ok )
      {
        switch ( CUR.error )
        {
        case TT_Err_Invalid_Opcode: /* looking for redefined instructions */
          A = 0;

          while ( A < CUR.numIDefs )
          {
            WITH = &CUR.IDefs[A];

            if ( WITH->Active && CUR.opcode == WITH->Opc )
            {
              if ( CUR.callTop >= CUR.callSize )
              {
                CUR.error = TT_Err_Invalid_Reference;
                goto _LErrorLabel;
              }

              WITH1 = &CUR.callStack[CUR.callTop];

              WITH1->Caller_Range = CUR.curRange;
              WITH1->Caller_IP    = CUR.IP + 1;
              WITH1->Cur_Count    = 1;
              WITH1->Cur_Restart  = WITH->Start;

              if ( INS_Goto_CodeRange( WITH->Range, WITH->Start ) == FAILURE )
                goto _LErrorLabel;

              goto _LSuiteLabel;
            }
            else
            {
              A++;
              continue;
            }
          }

          CUR.error = TT_Err_Invalid_Opcode;
          goto _LErrorLabel;
          break;

        default:
          goto _LErrorLabel;
          break;
        }
      }

      CUR.top = CUR.new_top;

      if ( CUR.step_ins )
        CUR.IP += CUR.length;

  _LSuiteLabel:

      if ( CUR.IP >= CUR.codeSize )
      {
        if ( CUR.callTop > 0 )
        {
          CUR.error = TT_Err_Code_Overflow;
          goto _LErrorLabel;
        }
        else
          goto _LNo_Error;
      }
    } while ( !CUR.instruction_trap );

  _LNo_Error:
    Result = TT_Err_Ok;
    goto _LExit;

  _LErrorLabel:
    Result = CUR.error;
    DBG_PRINT1("%%  ERROR=%d", Result);
  _LExit:
#   ifdef DEBUG
    if (save_ox != NULL) {
      mem->free(mem, save_ox, "RunIns");
      mem->free(mem, save_oy, "RunIns");
      mem->free(mem, save_cx, "RunIns");
      mem->free(mem, save_cy, "RunIns");
    }
#   endif

    return Result;
  }