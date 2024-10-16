static MagickBooleanType CheckPrimitiveExtent(MVGInfo *mvg_info,
  const double pad)
{
  double
    extent;

  size_t
    quantum;

  /*
    Check if there is enough storage for drawing pimitives.
  */
  quantum=sizeof(**mvg_info->primitive_info);
  extent=(double) mvg_info->offset+pad+(PrimitiveExtentPad+1)*quantum;
  if (extent <= (double) *mvg_info->extent)
    return(MagickTrue);
  if (extent == (double) CastDoubleToLong(extent))
    {
      *mvg_info->primitive_info=(PrimitiveInfo *) ResizeQuantumMemory(
        *mvg_info->primitive_info,(size_t) (extent+1),quantum);
      if (*mvg_info->primitive_info != (PrimitiveInfo *) NULL)
        {
          ssize_t
            i;

          *mvg_info->extent=(size_t) extent;
          for (i=mvg_info->offset+1; i <= (ssize_t) extent; i++)
          {
            (*mvg_info->primitive_info)[i].primitive=UndefinedPrimitive;
            (*mvg_info->primitive_info)[i].text=(char *) NULL;
          }
          return(MagickTrue);
        }
    }
  /*
    Reallocation failed, allocate a primitive to facilitate unwinding.
  */
  (void) ThrowMagickException(mvg_info->exception,GetMagickModule(),
    ResourceLimitError,"MemoryAllocationFailed","`%s'","");
  if (*mvg_info->primitive_info != (PrimitiveInfo *) NULL)
    *mvg_info->primitive_info=(PrimitiveInfo *) RelinquishMagickMemory(
      *mvg_info->primitive_info);
  *mvg_info->primitive_info=(PrimitiveInfo *) AcquireCriticalMemory((size_t) (
    (PrimitiveExtentPad+1)*quantum));
  (void) memset(*mvg_info->primitive_info,0,(size_t) ((PrimitiveExtentPad+1)*
    quantum));
  *mvg_info->extent=1;
  mvg_info->offset=0;
  return(MagickFalse);
}