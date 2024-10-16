void CLASS process_Sony_0x940e(uchar *buf, ushort len, unsigned id)
{
  if (((id == 286) || (id == 287) || (id == 294)) && (len >= 0x017e))
  {
    imgdata.makernotes.sony.AFMicroAdjValue = SonySubstitution[buf[0x017d]];
  }
  else if ((imgdata.makernotes.sony.SonyCameraType == LIBRAW_SONY_ILCA) && (len >= 0x0051))
  {
    imgdata.makernotes.sony.AFMicroAdjValue = SonySubstitution[buf[0x0050]];
  }
  else
    return;

  if (imgdata.makernotes.sony.AFMicroAdjValue != 0)
    imgdata.makernotes.sony.AFMicroAdjOn = 1;
}