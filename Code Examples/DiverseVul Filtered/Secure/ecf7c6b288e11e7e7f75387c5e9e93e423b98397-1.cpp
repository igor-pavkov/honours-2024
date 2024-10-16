MagickExport DrawInfo *AcquireDrawInfo(void)
{
  DrawInfo
    *draw_info;

  draw_info=(DrawInfo *) AcquireCriticalMemory(sizeof(*draw_info));
  GetDrawInfo((ImageInfo *) NULL,draw_info);
  return(draw_info);
}