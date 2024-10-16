static MagickBooleanType CheckPrimitiveExtent(MVGInfo *mvg_info,
  const size_t pad)
{
  double
    extent;

  size_t
    quantum;

  /*
    Check if there is enough storage for drawing pimitives.
  */
  extent=(double) mvg_info->offset+pad+PrimitiveExtentPad;
  quantum=sizeof(**mvg_info->primitive_info);
  if (((extent*quantum) < (double) SSIZE_MAX) &&
      ((extent*quantum) < (double) GetMaxMemoryRequest()))
    {
      if (extent <= (double) *mvg_info->extent)
        return(MagickTrue);
      *mvg_info->primitive_info=(PrimitiveInfo *) ResizeQuantumMemory(
        *mvg_info->primitive_info,(size_t) extent,quantum);
      if (*mvg_info->primitive_info != (PrimitiveInfo *) NULL)
        {
          register ssize_t
            i;

          *mvg_info->extent=(size_t) extent;
          for (i=mvg_info->offset+1; i < (ssize_t) extent; i++)
            (*mvg_info->primitive_info)[i].primitive=UndefinedPrimitive;
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
  *mvg_info->primitive_info=(PrimitiveInfo *) AcquireCriticalMemory(
    PrimitiveExtentPad*quantum);
  (void) memset(*mvg_info->primitive_info,0,PrimitiveExtentPad*quantum);
  *mvg_info->extent=1;
  return(MagickFalse);
}