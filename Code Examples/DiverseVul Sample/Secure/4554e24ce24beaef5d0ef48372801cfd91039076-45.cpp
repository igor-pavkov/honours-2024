void CLASS process_Sony_0x0116(uchar *buf, ushort len, unsigned id)
{
  short bufx;

  if (((id == 257) || (id == 262) || (id == 269) || (id == 270)) && (len >= 2))
    bufx = buf[1];
  else if ((id >= 273) && (len >= 3))
    bufx = buf[2];
  else
    return;

  imgdata.other.BatteryTemperature = (float)(bufx - 32) / 1.8f;
}