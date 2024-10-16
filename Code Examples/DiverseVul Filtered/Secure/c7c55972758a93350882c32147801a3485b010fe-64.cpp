  static void  Interp( Int  p1, Int  p2,
                       Int  ref1, Int  ref2,
                       struct LOC_Ins_IUP*  LINK )
  {
    Long        i;
    TT_F26Dot6  x, x1, x2, d1, d2;

    if ( p1 > p2 )
      return;

    x1 = LINK->orgs[ref1];
    d1 = LINK->curs[ref1] - LINK->orgs[ref1];
    x2 = LINK->orgs[ref2];
    d2 = LINK->curs[ref2] - LINK->orgs[ref2];

    if ( x1 == x2 )
    {
      for ( i = p1; i <= p2; i++ )
      {
        x = LINK->orgs[i];

        if ( x <= x1 )
          x += d1;
        else
          x += d2;

        LINK->curs[i] = x;
      }
      return;
    }

    if ( x1 < x2 )
    {
      for ( i = p1; i <= p2; i++ )
      {
        x = LINK->orgs[i];

        if ( x <= x1 )
          x += d1;
        else
        {
          if ( x >= x2 )
            x += d2;
          else
            x = LINK->curs[ref1] +
                  MulDiv_Round( x - x1,
                                LINK->curs[ref2] - LINK->curs[ref1],
                                x2 - x1 );
        }
        LINK->curs[i] = x;
      }
      return;
    }

    /* x2 < x1 */

    for ( i = p1; i <= p2; i++ )
    {
      x = LINK->orgs[i];
      if ( x <= x2 )
        x += d2;
      else
      {
        if ( x >= x1 )
          x += d1;
        else
          x = LINK->curs[ref1] +
                MulDiv_Round( x - x1,
                              LINK->curs[ref2] - LINK->curs[ref1],
                              x2 - x1 );
      }
      LINK->curs[i] = x;
    }
  }