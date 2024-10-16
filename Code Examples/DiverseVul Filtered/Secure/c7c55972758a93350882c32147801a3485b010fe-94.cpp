  static void  Compute_Funcs( EXEC_OP )
  {
    if ( CUR.GS.freeVector.x == 0x4000 )
    {
      CUR.func_freeProj = (TProject_Function)Project_x;
      CUR.F_dot_P       = CUR.GS.projVector.x * 0x10000L;
    }
    else
    {
      if ( CUR.GS.freeVector.y == 0x4000 )
      {
        CUR.func_freeProj = (TProject_Function)Project_y;
        CUR.F_dot_P       = CUR.GS.projVector.y * 0x10000L;
      }
      else
      {
        CUR.func_move     = (TMove_Function)Direct_Move;
        CUR.func_freeProj = (TProject_Function)Free_Project;
        CUR.F_dot_P = (Long)CUR.GS.projVector.x * CUR.GS.freeVector.x * 4 +
                      (Long)CUR.GS.projVector.y * CUR.GS.freeVector.y * 4;
      }
    }

    CUR.cached_metrics = FALSE;

    if ( CUR.GS.projVector.x == 0x4000 )
      CUR.func_project = (TProject_Function)Project_x;
    else
    {
      if ( CUR.GS.projVector.y == 0x4000 )
        CUR.func_project = (TProject_Function)Project_y;
      else
        CUR.func_project = (TProject_Function)Project;
    }

    if ( CUR.GS.dualVector.x == 0x4000 )
      CUR.func_dualproj = (TProject_Function)Project_x;
    else
    {
      if ( CUR.GS.dualVector.y == 0x4000 )
        CUR.func_dualproj = (TProject_Function)Project_y;
      else
        CUR.func_dualproj = (TProject_Function)Dual_Project;
    }

    CUR.func_move = (TMove_Function)Direct_Move;

    if ( CUR.F_dot_P == 0x40000000L )
    {
      if ( CUR.GS.freeVector.x == 0x4000 )
        CUR.func_move = (TMove_Function)Direct_Move_X;
      else
      {
        if ( CUR.GS.freeVector.y == 0x4000 )
          CUR.func_move = (TMove_Function)Direct_Move_Y;
      }
    }

    /* at small sizes, F_dot_P can become too small, resulting   */
    /* in overflows and 'spikes' in a number of glyphs like 'w'. */

    if ( ABS( CUR.F_dot_P ) < 0x4000000L )
      CUR.F_dot_P = 0x40000000L;

    /* Disable cached aspect ratio */
    CUR.metrics.ratio = 0;
  }