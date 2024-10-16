MagickExport Image *CoalesceImages(const Image *image,ExceptionInfo *exception)
{
  Image
    *coalesce_image,
    *dispose_image,
    *previous;

  register Image
    *next;

  RectangleInfo
    bounds;

  /*
    Coalesce the image sequence.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);

  /* initialise first image */
  next=GetFirstImageInList(image);
  bounds=next->page;
  if (bounds.width == 0)
    {
      bounds.width=next->columns;
      if (bounds.x > 0)
        bounds.width+=bounds.x;
    }
  if (bounds.height == 0)
    {
      bounds.height=next->rows;
      if (bounds.y > 0)
        bounds.height+=bounds.y;
    }
  bounds.x=0;
  bounds.y=0;
  coalesce_image=CloneImage(next,bounds.width,bounds.height,MagickTrue,
    exception);
  if (coalesce_image == (Image *) NULL)
    return((Image *) NULL);
  coalesce_image->background_color.opacity=(Quantum) TransparentOpacity;
  (void) SetImageBackgroundColor(coalesce_image);
  coalesce_image->matte=next->matte;
  coalesce_image->page=bounds;
  coalesce_image->dispose=NoneDispose;
  /*
    Coalesce rest of the images.
  */
  dispose_image=CloneImage(coalesce_image,0,0,MagickTrue,exception);
  (void) CompositeImage(coalesce_image,CopyCompositeOp,next,next->page.x,
    next->page.y);
  next=GetNextImageInList(next);
  for ( ; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    /*
      Determine the bounds that was overlaid in the previous image.
    */
    previous=GetPreviousImageInList(next);
    bounds=previous->page;
    bounds.width=previous->columns;
    bounds.height=previous->rows;
    if (bounds.x < 0)
      {
        bounds.width+=bounds.x;
        bounds.x=0;
      }
    if ((ssize_t) (bounds.x+bounds.width) > (ssize_t) coalesce_image->columns)
      bounds.width=coalesce_image->columns-bounds.x;
    if (bounds.y < 0)
      {
        bounds.height+=bounds.y;
        bounds.y=0;
      }
    if ((ssize_t) (bounds.y+bounds.height) > (ssize_t) coalesce_image->rows)
      bounds.height=coalesce_image->rows-bounds.y;
    /*
      Replace the dispose image with the new coalesced image.
    */
    if (GetPreviousImageInList(next)->dispose != PreviousDispose)
      {
        dispose_image=DestroyImage(dispose_image);
        dispose_image=CloneImage(coalesce_image,0,0,MagickTrue,exception);
        if (dispose_image == (Image *) NULL)
          {
            coalesce_image=DestroyImageList(coalesce_image);
            return((Image *) NULL);
          }
      }
    /*
      Clear the overlaid area of the coalesced bounds for background disposal
    */
    if (next->previous->dispose == BackgroundDispose)
      ClearBounds(dispose_image,&bounds);
    /*
      Next image is the dispose image, overlaid with next frame in sequence.
    */
    coalesce_image->next=CloneImage(dispose_image,0,0,MagickTrue,exception);
    if (coalesce_image->next != NULL)
      coalesce_image->next->previous=coalesce_image;
    previous=coalesce_image;
    coalesce_image=GetNextImageInList(coalesce_image);
    (void) CompositeImage(coalesce_image,next->matte != MagickFalse ?
      OverCompositeOp : CopyCompositeOp,next,next->page.x,next->page.y);
    (void) CloneImageProfiles(coalesce_image,next);
    (void) CloneImageProperties(coalesce_image,next);
    (void) CloneImageArtifacts(coalesce_image,next);
    coalesce_image->page=previous->page;
    /*
      If a pixel goes opaque to transparent, use background dispose.
    */
    if (IsBoundsCleared(previous,coalesce_image,&bounds,exception) != MagickFalse)
      coalesce_image->dispose=BackgroundDispose;
    else
      coalesce_image->dispose=NoneDispose;
    previous->dispose=coalesce_image->dispose;
  }
  dispose_image=DestroyImage(dispose_image);
  return(GetFirstImageInList(coalesce_image));
}