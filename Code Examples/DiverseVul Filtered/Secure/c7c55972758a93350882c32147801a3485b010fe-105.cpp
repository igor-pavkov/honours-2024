  static void  Ins_MIAP( INS_ARG )
  {
    Int         cvtEntry, point;
    TT_F26Dot6  distance,
                org_dist;

    cvtEntry = (Int)args[1];
    point    = (Int)args[0];

    if ( BOUNDS( args[0], CUR.zp0.n_points ) ||
         BOUNDS( args[1], CUR.cvtSize )      )
    {
        /* Ignore these errors, abort the instruction
         * and continue. This restores the FreeType
         * behaviour when pedantic_hinting is false. For bug
         * #689471, see also Ins_SHC above and bug #688501.
         */
      return;
    }

    /* Undocumented:                                     */
    /*                                                   */
    /* The behaviour of an MIAP instruction is quite     */
    /* different when used in the twilight zone.         */
    /*                                                   */
    /* First, no control value cutin test is performed   */
    /* as it would fail anyway.  Second, the original    */
    /* point, i.e. (org_x,org_y) of zp0.point, is set    */
    /* to the absolute, unrounded distance found in      */
    /* the CVT.                                          */
    /*                                                   */
    /* This is used in the CVT programs of the Microsoft */
    /* fonts Arial, Times, etc., in order to re-adjust   */
    /* some key font heights.  It allows the use of the  */
    /* IP instruction in the twilight zone, which        */
    /* otherwise would be "illegal" according to the     */
    /* specs :)                                          */
    /*                                                   */
    /* We implement it with a special sequence for the   */
    /* twilight zone. This is a bad hack, but it seems   */
    /* to work.                                          */

    distance = CUR_Func_read_cvt( cvtEntry );

    DBG_PRINT3(" cvtEntry=%d point=%d distance=%d", cvtEntry, point, distance);

    if ( CUR.GS.gep0 == 0 )   /* If in twilight zone */
    {
      CUR.zp0.org_x[point] = MulDiv_Round( CUR.GS.freeVector.x,
                                           distance, 0x4000L );
      CUR.zp0.cur_x[point] = CUR.zp0.org_x[point];

      CUR.zp0.org_y[point] = MulDiv_Round( CUR.GS.freeVector.y,
                                           distance, 0x4000L );
      CUR.zp0.cur_y[point] = CUR.zp0.org_y[point];
    }

    org_dist = CUR_Func_project( CUR.zp0.cur_x[point],
                                 CUR.zp0.cur_y[point] );

    if ( (CUR.opcode & 1) != 0 )   /* rounding and control cutin flag */
    {
      if ( ABS(distance - org_dist) > CUR.GS.control_value_cutin )
        distance = org_dist;

      distance = CUR_Func_round( distance, CUR.metrics.compensations[0] );
    }

    CUR_Func_move( &CUR.zp0, point, distance - org_dist );

    CUR.GS.rp0 = point;
    CUR.GS.rp1 = point;
  }