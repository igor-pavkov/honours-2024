  static void  Ins_MIRP( INS_ARG )
  {
    Int         point,
                cvtEntry;

    TT_F26Dot6  cvt_dist,
                distance,
                cur_dist,
                org_dist;

    point    = (Int)args[0];
    cvtEntry = (Int)args[1];

    /* XXX: UNDOCUMENTED! cvt[-1] = 0 always */

    if ( BOUNDS( args[0],   CUR.zp1.n_points ) ||
         BOUNDS( args[1]+1, CUR.cvtSize+1 )    )
    {
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    if ( args[1] < 0 )
      cvt_dist = 0;
    else
      cvt_dist = CUR_Func_read_cvt( cvtEntry );

    /* single width test */

    if ( ABS( cvt_dist ) < CUR.GS.single_width_cutin )
    {
      if ( cvt_dist >= 0 )
        cvt_dist =  CUR.GS.single_width_value;
      else
        cvt_dist = -CUR.GS.single_width_value;
    }

    /* XXX : Undocumented - twilight zone */

    if ( CUR.GS.gep1 == 0 )
    {
      CUR.zp1.org_x[point] = CUR.zp0.org_x[CUR.GS.rp0] +
                             MulDiv_Round( cvt_dist,
                                           CUR.GS.freeVector.x,
                                           0x4000 );

      CUR.zp1.org_y[point] = CUR.zp0.org_y[CUR.GS.rp0] +
                             MulDiv_Round( cvt_dist,
                                           CUR.GS.freeVector.y,
                                           0x4000 );

      CUR.zp1.cur_x[point] = CUR.zp1.org_x[point];
      CUR.zp1.cur_y[point] = CUR.zp1.org_y[point];
    }

    org_dist = CUR_Func_dualproj( CUR.zp1.org_x[point] -
                                    CUR.zp0.org_x[CUR.GS.rp0],
                                  CUR.zp1.org_y[point] -
                                    CUR.zp0.org_y[CUR.GS.rp0] );

    cur_dist = CUR_Func_project( CUR.zp1.cur_x[point] -
                                   CUR.zp0.cur_x[CUR.GS.rp0],
                                 CUR.zp1.cur_y[point] -
                                   CUR.zp0.cur_y[CUR.GS.rp0] );

    /* auto-flip test */

    if ( CUR.GS.auto_flip )
    {
      if ( (org_dist ^ cvt_dist) < 0 )
        cvt_dist = -cvt_dist;
    }

    /* control value cutin and round */

    if ( (CUR.opcode & 4) != 0 )
    {
      /* XXX: Undocumented : only perform cut-in test when both points */
      /*      refer to the same zone.                                  */

      if ( CUR.GS.gep0 == CUR.GS.gep1 )
        if ( ABS( cvt_dist - org_dist ) >= CUR.GS.control_value_cutin )
          cvt_dist = org_dist;

      distance = CUR_Func_round( cvt_dist,
                                 CUR.metrics.compensations[CUR.opcode & 3] );
    }
    else
      distance = Round_None( EXEC_ARGS
                             cvt_dist,
                             CUR.metrics.compensations[CUR.opcode & 3] );

    /* minimum distance test */

    if ( (CUR.opcode & 8) != 0 )
    {
      if ( org_dist >= 0 )
      {
        if ( distance < CUR.GS.minimum_distance )
          distance = CUR.GS.minimum_distance;
      }
      else
      {
        if ( distance > -CUR.GS.minimum_distance )
          distance = -CUR.GS.minimum_distance;
      }
    }

    CUR_Func_move( &CUR.zp1, point, distance - cur_dist );

    CUR.GS.rp1 = CUR.GS.rp0;

    if ( (CUR.opcode & 16) != 0 )
      CUR.GS.rp0 = point;

    /* UNDOCUMENTED! */

    CUR.GS.rp2 = point;
  }