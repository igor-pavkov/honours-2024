void CLASS process_Sony_0x9402(uchar *buf, ushort len)
{

  if ((imgdata.makernotes.sony.SonyCameraType == LIBRAW_SONY_SLT) ||
      (imgdata.makernotes.sony.SonyCameraType == LIBRAW_SONY_ILCA))
    return;

  if (len < 5)
    return;

  short bufx = buf[0x00];
  if ((bufx == 0x05) || (bufx == 0xff) || (buf[0x02] != 0xff))
    return;

  imgdata.other.AmbientTemperature = (float)((short)SonySubstitution[buf[0x04]]);

  return;
}