static void CalcMinMax(Image *image, int endian_indicator, int SizeX, int SizeY, size_t CellType, unsigned ldblk, void *BImgBuff, double *Min, double *Max)
{
MagickOffsetType filepos;
int i, x;
void (*ReadBlobDoublesXXX)(Image * image, size_t len, double *data);
void (*ReadBlobFloatsXXX)(Image * image, size_t len, float *data);
double *dblrow;
float *fltrow;

  if (endian_indicator == LSBEndian)
  {
    ReadBlobDoublesXXX = ReadBlobDoublesLSB;
    ReadBlobFloatsXXX = ReadBlobFloatsLSB;
  }
  else    /* MI */
  {
    ReadBlobDoublesXXX = ReadBlobDoublesMSB;
    ReadBlobFloatsXXX = ReadBlobFloatsMSB;
  }

  filepos = TellBlob(image);     /* Please note that file seeking occurs only in the case of doubles */
  for (i = 0; i < SizeY; i++)
  {
    if (CellType==miDOUBLE)
    {
      ReadBlobDoublesXXX(image, ldblk, (double *)BImgBuff);
      dblrow = (double *)BImgBuff;
      if (i == 0)
      {
        *Min = *Max = *dblrow;
      }
      for (x = 0; x < SizeX; x++)
      {
        if (*Min > *dblrow)
          *Min = *dblrow;
        if (*Max < *dblrow)
          *Max = *dblrow;
        dblrow++;
      }
    }
    if (CellType==miSINGLE)
    {
      ReadBlobFloatsXXX(image, ldblk, (float *)BImgBuff);
      fltrow = (float *)BImgBuff;
      if (i == 0)
      {
        *Min = *Max = *fltrow;
      }
    for (x = 0; x < (ssize_t) SizeX; x++)
      {
        if (*Min > *fltrow)
          *Min = *fltrow;
        if (*Max < *fltrow)
          *Max = *fltrow;
        fltrow++;
      }
    }
  }
  (void) SeekBlob(image, filepos, SEEK_SET);
}