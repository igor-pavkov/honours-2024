void CLASS parse_gps_libraw(int base)
{
  unsigned entries, tag, type, len, save, c;

  entries = get2();
  if (entries > 200)
    return;
  if (entries > 0)
    imgdata.other.parsed_gps.gpsparsed = 1;
  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    if (len > 1024)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue; // no GPS tags are 1k or larger
    }
    switch (tag)
    {
    case 1:
      imgdata.other.parsed_gps.latref = getc(ifp);
      break;
    case 3:
      imgdata.other.parsed_gps.longref = getc(ifp);
      break;
    case 5:
      imgdata.other.parsed_gps.altref = getc(ifp);
      break;
    case 2:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.latitude[c] = getreal(type);
      break;
    case 4:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.longtitude[c] = getreal(type);
      break;
    case 7:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.gpstimestamp[c] = getreal(type);
      break;
    case 6:
      imgdata.other.parsed_gps.altitude = getreal(type);
      break;
    case 9:
      imgdata.other.parsed_gps.gpsstatus = getc(ifp);
      break;
    }
    fseek(ifp, save, SEEK_SET);
  }
}