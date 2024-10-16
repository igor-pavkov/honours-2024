  static void  Ins_IUP( INS_ARG )
  {
    struct LOC_Ins_IUP  V;
    unsigned char       mask;

    Long first_point;   /* first point of contour        */
    Long end_point;     /* end point (last+1) of contour */

    Long first_touched; /* first touched point in contour   */
    Long cur_touched;   /* current touched point in contour */

    Long point;         /* current point   */
    Long contour;       /* current contour */
    (void)args;

    if ( CUR.opcode & 1 )
    {
      mask   = TT_Flag_Touched_X;
      V.orgs = CUR.pts.org_x;
      V.curs = CUR.pts.cur_x;
    }
    else
    {
      mask   = TT_Flag_Touched_Y;
      V.orgs = CUR.pts.org_y;
      V.curs = CUR.pts.cur_y;
    }

    contour = 0;
    point   = 0;

    do
    {
      end_point   = CUR.pts.contours[contour];
      first_point = point;

      while ( point <= end_point && (CUR.pts.touch[point] & mask) == 0 )
        point++;

      if ( point <= end_point )
      {
        first_touched = point;
        cur_touched   = point;

        point++;

        while ( point <= end_point )
        {
          if ( (CUR.pts.touch[point] & mask) != 0 )
          {
            Interp( (Int)(cur_touched + 1),
                    (Int)(point - 1),
                    (Int)cur_touched,
                    (Int)point,
                    &V );
            cur_touched = point;
          }

          point++;
        }

        if ( cur_touched == first_touched )
          Shift( (Int)first_point, (Int)end_point, (Int)cur_touched, &V );
        else
        {
          Interp((Int)(cur_touched + 1),
                 (Int)(end_point),
                 (Int)(cur_touched),
                 (Int)(first_touched),
                 &V );

          Interp((Int)(first_point),
                 (Int)(first_touched - 1),
                 (Int)(cur_touched),
                 (Int)(first_touched),
                 &V );
        }
      }
      contour++;
    } while ( contour < CUR.pts.n_contours );
  }