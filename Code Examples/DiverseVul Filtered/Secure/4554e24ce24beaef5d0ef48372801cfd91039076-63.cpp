void CLASS parse_gps(int base)
{
  unsigned entries, tag, type, len, save, c;

  entries = get2();
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
    case 3:
    case 5:
      gpsdata[29 + tag / 2] = getc(ifp);
      break;
    case 2:
    case 4:
    case 7:
      FORC(6) gpsdata[tag / 3 * 6 + c] = get4();
      break;
    case 6:
      FORC(2) gpsdata[18 + c] = get4();
      break;
    case 18:
    case 29:
      fgets((char *)(gpsdata + 14 + tag / 3), MIN(len, 12), ifp);
    }
    fseek(ifp, save, SEEK_SET);
  }
}