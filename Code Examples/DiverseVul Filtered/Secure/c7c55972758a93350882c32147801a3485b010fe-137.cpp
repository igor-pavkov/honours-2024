  static void  Shift( Int  p1,
                      Int  p2,
                      Int  p,
                      struct LOC_Ins_IUP*  LINK )
  {
    Int         i;
    TT_F26Dot6  x;

    x = LINK->curs[p] - LINK->orgs[p];

    for ( i = p1; i < p; i++ )
      LINK->curs[i] += x;

    for ( i = p + 1; i <= p2; i++ )
      LINK->curs[i] += x;
  }