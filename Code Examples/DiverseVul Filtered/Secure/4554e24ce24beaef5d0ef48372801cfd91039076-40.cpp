void CLASS process_Sony_0x2010(uchar *buf, ushort len)
{
  if ((!imgdata.makernotes.sony.group2010) || (imgdata.makernotes.sony.real_iso_offset == 0xffff) ||
      (len < (imgdata.makernotes.sony.real_iso_offset + 2)))
    return;

  if (imgdata.other.real_ISO < 0.1f)
  {
    uchar s[2];
    s[0] = SonySubstitution[buf[imgdata.makernotes.sony.real_iso_offset]];
    s[1] = SonySubstitution[buf[imgdata.makernotes.sony.real_iso_offset + 1]];
    imgdata.other.real_ISO = 100.0f * libraw_powf64l(2.0f, (16 - ((float)sget2(s)) / 256.0f));
  }
}