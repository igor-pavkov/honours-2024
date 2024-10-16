MagickExport MagickBooleanType SetImageOption(ImageInfo *image_info,
  const char *option,const char *value)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  /*
    Specific global option settings.
  */
  if (LocaleCompare(option,"size") == 0) {
    (void) CloneString(&image_info->size,value);
    return(MagickTrue);
  }
  /*
    Create tree if needed - specify how key,values are to be freed.
  */
  if (image_info->options == (void *) NULL)
    image_info->options=NewSplayTree(CompareSplayTreeString,
      RelinquishMagickMemory,RelinquishMagickMemory);
  /*
    Delete Option if NULL --  empty string values are valid!
  */
  if (value == (const char *) NULL)
    return(DeleteImageOption(image_info,option));
  /*
    Add option to splay-tree.
  */
  return(AddValueToSplayTree((SplayTreeInfo *) image_info->options,
    ConstantString(option),ConstantString(value)));
}