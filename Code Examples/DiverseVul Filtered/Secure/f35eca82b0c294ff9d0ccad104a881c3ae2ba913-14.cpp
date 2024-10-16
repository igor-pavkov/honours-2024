MagickExport char *GetPageGeometry(const char *page_geometry)
{
#define MagickPageSize(name,geometry) { (name), sizeof(name)-1, (geometry) }

  typedef struct _PageInfo
  {
    const char
      name[12];

    size_t
      extent;

    const char
      geometry[10];
  } PageInfo;

  static const PageInfo
    PageSizes[] =
    {
      MagickPageSize("4x6", "288x432"),
      MagickPageSize("5x7", "360x504"),
      MagickPageSize("7x9", "504x648"),
      MagickPageSize("8x10", "576x720"),
      MagickPageSize("9x11", "648x792"),
      MagickPageSize("9x12", "648x864"),
      MagickPageSize("10x13", "720x936"),
      MagickPageSize("10x14", "720x1008"),
      MagickPageSize("11x17", "792x1224"),
      MagickPageSize("a0", "2384x3370"),
      MagickPageSize("a1", "1684x2384"),
      MagickPageSize("a10", "73x105"),
      MagickPageSize("a2", "1191x1684"),
      MagickPageSize("a3", "842x1191"),
      MagickPageSize("a4", "595x842"),
      MagickPageSize("a4small", "595x842"),
      MagickPageSize("a5", "420x595"),
      MagickPageSize("a6", "297x420"),
      MagickPageSize("a7", "210x297"),
      MagickPageSize("a8", "148x210"),
      MagickPageSize("a9", "105x148"),
      MagickPageSize("archa", "648x864"),
      MagickPageSize("archb", "864x1296"),
      MagickPageSize("archC", "1296x1728"),
      MagickPageSize("archd", "1728x2592"),
      MagickPageSize("arche", "2592x3456"),
      MagickPageSize("b0", "2920x4127"),
      MagickPageSize("b1", "2064x2920"),
      MagickPageSize("b10", "91x127"),
      MagickPageSize("b2", "1460x2064"),
      MagickPageSize("b3", "1032x1460"),
      MagickPageSize("b4", "729x1032"),
      MagickPageSize("b5", "516x729"),
      MagickPageSize("b6", "363x516"),
      MagickPageSize("b7", "258x363"),
      MagickPageSize("b8", "181x258"),
      MagickPageSize("b9", "127x181"),
      MagickPageSize("c0", "2599x3676"),
      MagickPageSize("c1", "1837x2599"),
      MagickPageSize("c2", "1298x1837"),
      MagickPageSize("c3", "918x1296"),
      MagickPageSize("c4", "649x918"),
      MagickPageSize("c5", "459x649"),
      MagickPageSize("c6", "323x459"),
      MagickPageSize("c7", "230x323"),
      MagickPageSize("csheet", "1224x1584"),
      MagickPageSize("dsheet", "1584x2448"),
      MagickPageSize("esheet", "2448x3168"),
      MagickPageSize("executive", "540x720"),
      MagickPageSize("flsa", "612x936"),
      MagickPageSize("flse", "612x936"),
      MagickPageSize("folio", "612x936"),
      MagickPageSize("halfletter", "396x612"),
      MagickPageSize("isob0", "2835x4008"),
      MagickPageSize("isob1", "2004x2835"),
      MagickPageSize("isob10", "88x125"),
      MagickPageSize("isob2", "1417x2004"),
      MagickPageSize("isob3", "1001x1417"),
      MagickPageSize("isob4", "709x1001"),
      MagickPageSize("isob5", "499x709"),
      MagickPageSize("isob6", "354x499"),
      MagickPageSize("isob7", "249x354"),
      MagickPageSize("isob8", "176x249"),
      MagickPageSize("isob9", "125x176"),
      MagickPageSize("jisb0", "1030x1456"),
      MagickPageSize("jisb1", "728x1030"),
      MagickPageSize("jisb2", "515x728"),
      MagickPageSize("jisb3", "364x515"),
      MagickPageSize("jisb4", "257x364"),
      MagickPageSize("jisb5", "182x257"),
      MagickPageSize("jisb6", "128x182"),
      MagickPageSize("ledger", "1224x792"),
      MagickPageSize("legal", "612x1008"),
      MagickPageSize("letter", "612x792"),
      MagickPageSize("lettersmall", "612x792"),
      MagickPageSize("monarch", "279x540"),
      MagickPageSize("quarto", "610x780"),
      MagickPageSize("statement", "396x612"),
      MagickPageSize("tabloid", "792x1224")
    };

  char
    page[MaxTextExtent];

  register ssize_t
    i;

  assert(page_geometry != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",page_geometry);
  (void) CopyMagickString(page,page_geometry,MaxTextExtent);
  for (i=0; i < (ssize_t) (sizeof(PageSizes)/sizeof(PageSizes[0])); i++)
  {
    int
      status;

    status=LocaleNCompare(PageSizes[i].name,page_geometry,PageSizes[i].extent);
    if (status == 0)
      {
        MagickStatusType
          flags;

        RectangleInfo
          geometry;

        /*
          Replace mneumonic with the equivalent size in dots-per-inch.
        */
        (void) FormatLocaleString(page,MaxTextExtent,"%s%.80s",
          PageSizes[i].geometry,page_geometry+PageSizes[i].extent);
        flags=GetGeometry(page,&geometry.x,&geometry.y,&geometry.width,
          &geometry.height);
        if ((flags & GreaterValue) == 0)
          (void) ConcatenateMagickString(page,">",MaxTextExtent);
        break;
      }
  }
  return(AcquireString(page));
}