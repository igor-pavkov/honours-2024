MagickExport MagickStatusType ParseAffineGeometry(const char *geometry,
  AffineMatrix *affine_matrix,ExceptionInfo *exception)
{
  char
    token[MagickPathExtent];

  const char
    *p;

  double
    determinant;

  MagickStatusType
    flags;

  register ssize_t
    i;

  GetAffineMatrix(affine_matrix);
  flags=NoValue;
  p=(char *) geometry;
  for (i=0; (*p != '\0') && (i < 6); i++)
  {
    (void) GetNextToken(p,&p,MagickPathExtent,token);
    if (*token == ',')
      (void) GetNextToken(p,&p,MagickPathExtent,token);
    switch (i)
    {
      case 0:
      {
        affine_matrix->sx=StringToDouble(token,(char **) NULL);
        break;
      }
      case 1:
      {
        affine_matrix->rx=StringToDouble(token,(char **) NULL);
        break;
      }
      case 2:
      {
        affine_matrix->ry=StringToDouble(token,(char **) NULL);
        break;
      }
      case 3:
      {
        affine_matrix->sy=StringToDouble(token,(char **) NULL);
        break;
      }
      case 4:
      {
        affine_matrix->tx=StringToDouble(token,(char **) NULL);
        flags|=XValue;
        break;
      }
      case 5:
      {
        affine_matrix->ty=StringToDouble(token,(char **) NULL);
        flags|=YValue;
        break;
      }
    }
  }
  determinant=(affine_matrix->sx*affine_matrix->sy-affine_matrix->rx*
    affine_matrix->ry);
  if (fabs(determinant) < MagickEpsilon)
    (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
      "InvalidArgument","'%s' : 'Indeterminate Matrix'",geometry);
  return(flags);
}