static SVGInfo *AcquireSVGInfo(void)
{
  SVGInfo
    *svg_info;

  svg_info=(SVGInfo *) AcquireMagickMemory(sizeof(*svg_info));
  if (svg_info == (SVGInfo *) NULL)
    return((SVGInfo *) NULL);
  (void) memset(svg_info,0,sizeof(*svg_info));
  svg_info->text=AcquireString("");
  svg_info->scale=(double *) AcquireCriticalMemory(sizeof(*svg_info->scale));
  GetAffineMatrix(&svg_info->affine);
  svg_info->scale[0]=ExpandAffine(&svg_info->affine);
  return(svg_info);
}