static void Scale2X(const Image *source,const Quantum *pixels,Quantum *result,
  const size_t channels)
{
  if (PixelsEqual(pixels,1,pixels,7,channels) ||
      PixelsEqual(pixels,3,pixels,5,channels))
    {
      register ssize_t
        i;

      for (i=0; i < 4; i++)
        CopyPixels(pixels,4,result,i,channels);
      return;
    }
    if (PixelsEqual(pixels,1,pixels,3,channels))
      CopyPixels(pixels,3,result,0,channels);
    else
      CopyPixels(pixels,4,result,0,channels);
    if (PixelsEqual(pixels,1,pixels,5,channels))
      CopyPixels(pixels,5,result,1,channels);
    else
      CopyPixels(pixels,4,result,1,channels);
    if (PixelsEqual(pixels,3,pixels,7,channels))
      CopyPixels(pixels,3,result,2,channels);
    else
      CopyPixels(pixels,4,result,2,channels);
    if (PixelsEqual(pixels,5,pixels,7,channels))
      CopyPixels(pixels,5,result,3,channels);
    else
      CopyPixels(pixels,4,result,3,channels);
}