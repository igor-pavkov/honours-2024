static ssize_t FindColor(PixelInfo *packet)
{
  register ssize_t
    i;

  for (i=0; i < 256; i++)
    if (ScaleQuantumToChar(ClampToQuantum(packet->red)) == PalmPalette[i][0] &&
        ScaleQuantumToChar(ClampToQuantum(packet->green)) == PalmPalette[i][1] &&
        ScaleQuantumToChar(ClampToQuantum(packet->blue)) == PalmPalette[i][2])
      return(i);
  return(-1);
}