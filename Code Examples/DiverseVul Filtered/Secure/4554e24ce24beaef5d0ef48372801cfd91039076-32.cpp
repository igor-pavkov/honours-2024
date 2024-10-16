void CLASS Canon_WBpresets(int skip1, int skip2)
{
  int c;
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Daylight][c ^ (c >> 1)] = get2();

  if (skip1)
    fseek(ifp, skip1, SEEK_CUR);
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][c ^ (c >> 1)] = get2();

  if (skip1)
    fseek(ifp, skip1, SEEK_CUR);
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Cloudy][c ^ (c >> 1)] = get2();

  if (skip1)
    fseek(ifp, skip1, SEEK_CUR);
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c ^ (c >> 1)] = get2();

  if (skip1)
    fseek(ifp, skip1, SEEK_CUR);
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_W][c ^ (c >> 1)] = get2();

  if (skip2)
    fseek(ifp, skip2, SEEK_CUR);
  FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][c ^ (c >> 1)] = get2();

  return;
}