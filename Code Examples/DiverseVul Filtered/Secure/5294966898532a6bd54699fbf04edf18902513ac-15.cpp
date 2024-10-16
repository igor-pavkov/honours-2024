MagickExport QuantizeInfo *AcquireQuantizeInfo(const ImageInfo *image_info)
{
  QuantizeInfo
    *quantize_info;

  quantize_info=(QuantizeInfo *) AcquireCriticalMemory(sizeof(*quantize_info));
  GetQuantizeInfo(quantize_info);
  if (image_info != (ImageInfo *) NULL)
    {
      const char
        *option;

      quantize_info->dither_method=image_info->dither == MagickFalse ?
        NoDitherMethod : RiemersmaDitherMethod;
      option=GetImageOption(image_info,"dither");
      if (option != (const char *) NULL)
        quantize_info->dither_method=(DitherMethod) ParseCommandOption(
          MagickDitherOptions,MagickFalse,option);
      quantize_info->measure_error=image_info->verbose;
    }
  return(quantize_info);
}