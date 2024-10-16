  static void  SetSuperRound( EXEC_OPS TT_F26Dot6  GridPeriod,
                                       Long        selector )
  {
    switch ( selector & 0xC0 )
    {
      case 0:
        CUR.period = GridPeriod / 2;
        break;

      case 0x40:
        CUR.period = GridPeriod;
        break;

      case 0x80:
        CUR.period = GridPeriod * 2;
        break;

      /* This opcode is reserved, but... */

      case 0xC0:
        CUR.period = GridPeriod;
        break;
    }

    switch ( selector & 0x30 )
    {
    case 0:
      CUR.phase = 0;
      break;

    case 0x10:
      CUR.phase = CUR.period / 4;
      break;

    case 0x20:
      CUR.phase = CUR.period / 2;
      break;

    case 0x30:
      CUR.phase = GridPeriod * 3 / 4;
      break;
    }

    if ( (selector & 0x0F) == 0 )
      CUR.threshold = CUR.period - 1;
    else
      CUR.threshold = ( (Int)(selector & 0x0F) - 4L ) * CUR.period / 8;

    CUR.period    /= 256;
    CUR.phase     /= 256;
    CUR.threshold /= 256;
  }