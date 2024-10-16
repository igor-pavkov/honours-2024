MagickExport Image *ScaleImage(const Image *image,const size_t columns,
  const size_t rows,ExceptionInfo *exception)
{
#define ScaleImageTag  "Scale/Image"

  CacheView
    *image_view,
    *scale_view;

  double
    alpha,
    pixel[CompositePixelChannel],
    *scale_scanline,
    *scanline,
    *x_vector,
    *y_vector;

  Image
    *scale_image;

  MagickBooleanType
    next_column,
    next_row,
    proceed,
    status;

  PixelTrait
    scale_traits;

  PointInfo
    scale,
    span;

  register ssize_t
    i;

  ssize_t
    n,
    number_rows,
    y;

  /*
    Initialize scaled image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(ImageError,"NegativeOrZeroImageSize");
  if ((columns == image->columns) && (rows == image->rows))
    return(CloneImage(image,0,0,MagickTrue,exception));
  scale_image=CloneImage(image,columns,rows,MagickTrue,exception);
  if (scale_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(scale_image,DirectClass,exception) == MagickFalse)
    {
      scale_image=DestroyImage(scale_image);
      return((Image *) NULL);
    }
  /*
    Allocate memory.
  */
  x_vector=(double *) AcquireQuantumMemory((size_t) image->columns,
    MaxPixelChannels*sizeof(*x_vector));
  scanline=x_vector;
  if (image->rows != scale_image->rows)
    scanline=(double *) AcquireQuantumMemory((size_t) image->columns,
      MaxPixelChannels*sizeof(*scanline));
  scale_scanline=(double *) AcquireQuantumMemory((size_t) scale_image->columns,
    MaxPixelChannels*sizeof(*scale_scanline));
  y_vector=(double *) AcquireQuantumMemory((size_t) image->columns,
    MaxPixelChannels*sizeof(*y_vector));
  if ((scanline == (double *) NULL) || (scale_scanline == (double *) NULL) ||
      (x_vector == (double *) NULL) || (y_vector == (double *) NULL))
    {
      if ((image->rows != scale_image->rows) && (scanline != (double *) NULL))
        scanline=(double *) RelinquishMagickMemory(scanline);
      if (scale_scanline != (double *) NULL)
        scale_scanline=(double *) RelinquishMagickMemory(scale_scanline);
      if (x_vector != (double *) NULL)
        x_vector=(double *) RelinquishMagickMemory(x_vector);
      if (y_vector != (double *) NULL)
        y_vector=(double *) RelinquishMagickMemory(y_vector);
      scale_image=DestroyImage(scale_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Scale image.
  */
  number_rows=0;
  next_row=MagickTrue;
  span.y=1.0;
  scale.y=(double) scale_image->rows/(double) image->rows;
  (void) memset(y_vector,0,(size_t) MaxPixelChannels*image->columns*
    sizeof(*y_vector));
  n=0;
  status=MagickTrue;
  image_view=AcquireVirtualCacheView(image,exception);
  scale_view=AcquireAuthenticCacheView(scale_image,exception);
  for (y=0; y < (ssize_t) scale_image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      break;
    q=QueueCacheViewAuthenticPixels(scale_view,0,y,scale_image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        break;
      }
    alpha=1.0;
    if (scale_image->rows == image->rows)
      {
        /*
          Read a new scanline.
        */
        p=GetCacheViewVirtualPixels(image_view,0,n++,image->columns,1,
          exception);
        if (p == (const Quantum *) NULL)
          {
            status=MagickFalse;
            break;
          }
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          if (GetPixelWriteMask(image,p) <= (QuantumRange/2))
            {
              p+=GetPixelChannels(image);
              continue;
            }
          if (image->alpha_trait != UndefinedPixelTrait)
            alpha=QuantumScale*GetPixelAlpha(image,p);
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(image,i);
            PixelTrait traits = GetPixelChannelTraits(image,channel);
            if ((traits & BlendPixelTrait) == 0)
              {
                x_vector[x*GetPixelChannels(image)+i]=(double) p[i];
                continue;
              }
            x_vector[x*GetPixelChannels(image)+i]=alpha*p[i];
          }
          p+=GetPixelChannels(image);
        }
      }
    else
      {
        /*
          Scale Y direction.
        */
        while (scale.y < span.y)
        {
          if ((next_row != MagickFalse) &&
              (number_rows < (ssize_t) image->rows))
            {
              /*
                Read a new scanline.
              */
              p=GetCacheViewVirtualPixels(image_view,0,n++,image->columns,1,
                exception);
              if (p == (const Quantum *) NULL)
                {
                  status=MagickFalse;
                  break;
                }
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                if (GetPixelWriteMask(image,p) <= (QuantumRange/2))
                  {
                    p+=GetPixelChannels(image);
                    continue;
                  }
                if (image->alpha_trait != UndefinedPixelTrait)
                  alpha=QuantumScale*GetPixelAlpha(image,p);
                for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
                {
                  PixelChannel channel = GetPixelChannelChannel(image,i);
                  PixelTrait traits = GetPixelChannelTraits(image,channel);
                  if ((traits & BlendPixelTrait) == 0)
                    {
                      x_vector[x*GetPixelChannels(image)+i]=(double) p[i];
                      continue;
                    }
                  x_vector[x*GetPixelChannels(image)+i]=alpha*p[i];
                }
                p+=GetPixelChannels(image);
              }
              number_rows++;
            }
          for (x=0; x < (ssize_t) image->columns; x++)
            for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
              y_vector[x*GetPixelChannels(image)+i]+=scale.y*
                x_vector[x*GetPixelChannels(image)+i];
          span.y-=scale.y;
          scale.y=(double) scale_image->rows/(double) image->rows;
          next_row=MagickTrue;
        }
        if ((next_row != MagickFalse) && (number_rows < (ssize_t) image->rows))
          {
            /*
              Read a new scanline.
            */
            p=GetCacheViewVirtualPixels(image_view,0,n++,image->columns,1,
              exception);
            if (p == (const Quantum *) NULL)
              {
                status=MagickFalse;
                break;
              }
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (GetPixelWriteMask(image,p) <= (QuantumRange/2))
                {
                  p+=GetPixelChannels(image);
                  continue;
                }
              if (image->alpha_trait != UndefinedPixelTrait)
                alpha=QuantumScale*GetPixelAlpha(image,p);
              for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
              {
                PixelChannel channel = GetPixelChannelChannel(image,i);
                PixelTrait traits = GetPixelChannelTraits(image,channel);
                if ((traits & BlendPixelTrait) == 0)
                  {
                    x_vector[x*GetPixelChannels(image)+i]=(double) p[i];
                    continue;
                  }
                x_vector[x*GetPixelChannels(image)+i]=alpha*p[i];
              }
              p+=GetPixelChannels(image);
            }
            number_rows++;
            next_row=MagickFalse;
          }
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            pixel[i]=y_vector[x*GetPixelChannels(image)+i]+span.y*
              x_vector[x*GetPixelChannels(image)+i];
            scanline[x*GetPixelChannels(image)+i]=pixel[i];
            y_vector[x*GetPixelChannels(image)+i]=0.0;
          }
        }
        scale.y-=span.y;
        if (scale.y <= 0)
          {
            scale.y=(double) scale_image->rows/(double) image->rows;
            next_row=MagickTrue;
          }
        span.y=1.0;
      }
    if (scale_image->columns == image->columns)
      {
        /*
          Transfer scanline to scaled image.
        */
        for (x=0; x < (ssize_t) scale_image->columns; x++)
        {
          if (GetPixelWriteMask(scale_image,q) <= (QuantumRange/2))
            {
              q+=GetPixelChannels(scale_image);
              continue;
            }
          if (image->alpha_trait != UndefinedPixelTrait)
            {
              alpha=QuantumScale*scanline[x*GetPixelChannels(image)+
                GetPixelChannelOffset(image,AlphaPixelChannel)];
              alpha=PerceptibleReciprocal(alpha);
            }
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(image,i);
            PixelTrait traits = GetPixelChannelTraits(image,channel);
            scale_traits=GetPixelChannelTraits(scale_image,channel);
            if ((traits == UndefinedPixelTrait) ||
                (scale_traits == UndefinedPixelTrait))
              continue;
            if ((traits & BlendPixelTrait) == 0)
              {
                SetPixelChannel(scale_image,channel,ClampToQuantum(
                  scanline[x*GetPixelChannels(image)+i]),q);
                continue;
              }
            SetPixelChannel(scale_image,channel,ClampToQuantum(alpha*scanline[
              x*GetPixelChannels(image)+i]),q);
          }
          q+=GetPixelChannels(scale_image);
        }
      }
    else
      {
        ssize_t
          t;

        /*
          Scale X direction.
        */
        for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          pixel[i]=0.0;
        next_column=MagickFalse;
        span.x=1.0;
        t=0;
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          scale.x=(double) scale_image->columns/(double) image->columns;
          while (scale.x >= span.x)
          {
            if (next_column != MagickFalse)
              {
                for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
                  pixel[i]=0.0;
                t++;
              }
            for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
            {
              PixelChannel channel = GetPixelChannelChannel(image,i);
              PixelTrait traits = GetPixelChannelTraits(image,channel);
              if (traits == UndefinedPixelTrait)
                continue;
              pixel[i]+=span.x*scanline[x*GetPixelChannels(image)+i];
              scale_scanline[t*GetPixelChannels(image)+i]=pixel[i];
            }
            scale.x-=span.x;
            span.x=1.0;
            next_column=MagickTrue;
          }
          if (scale.x > 0)
            {
              if (next_column != MagickFalse)
                {
                  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
                    pixel[i]=0.0;
                  next_column=MagickFalse;
                  t++;
                }
              for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
                pixel[i]+=scale.x*scanline[x*GetPixelChannels(image)+i];
              span.x-=scale.x;
            }
        }
      if (span.x > 0)
        {
          for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
            pixel[i]+=span.x*scanline[(x-1)*GetPixelChannels(image)+i];
        }
      if ((next_column == MagickFalse) && (t < (ssize_t) scale_image->columns))
        for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
          scale_scanline[t*GetPixelChannels(image)+i]=pixel[i];
      /*
        Transfer scanline to scaled image.
      */
      for (x=0; x < (ssize_t) scale_image->columns; x++)
      {
        if (GetPixelWriteMask(scale_image,q) <= (QuantumRange/2))
          {
            q+=GetPixelChannels(scale_image);
            continue;
          }
        if (image->alpha_trait != UndefinedPixelTrait)
          {
            alpha=QuantumScale*scale_scanline[x*GetPixelChannels(image)+
              GetPixelChannelOffset(image,AlphaPixelChannel)];
            alpha=PerceptibleReciprocal(alpha);
          }
        for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
        {
          PixelChannel channel = GetPixelChannelChannel(image,i);
          PixelTrait traits = GetPixelChannelTraits(image,channel);
          scale_traits=GetPixelChannelTraits(scale_image,channel);
          if ((traits == UndefinedPixelTrait) ||
              (scale_traits == UndefinedPixelTrait))
            continue;
          if ((traits & BlendPixelTrait) == 0)
            {
              SetPixelChannel(scale_image,channel,ClampToQuantum(
                scale_scanline[x*GetPixelChannels(image)+i]),q);
              continue;
            }
          SetPixelChannel(scale_image,channel,ClampToQuantum(alpha*
            scale_scanline[x*GetPixelChannels(image)+i]),q);
        }
        q+=GetPixelChannels(scale_image);
      }
    }
    if (SyncCacheViewAuthenticPixels(scale_view,exception) == MagickFalse)
      {
        status=MagickFalse;
        break;
      }
    proceed=SetImageProgress(image,ScaleImageTag,(MagickOffsetType) y,
      image->rows);
    if (proceed == MagickFalse)
      {
        status=MagickFalse;
        break;
      }
  }
  scale_view=DestroyCacheView(scale_view);
  image_view=DestroyCacheView(image_view);
  /*
    Free allocated memory.
  */
  y_vector=(double *) RelinquishMagickMemory(y_vector);
  scale_scanline=(double *) RelinquishMagickMemory(scale_scanline);
  if (scale_image->rows != image->rows)
    scanline=(double *) RelinquishMagickMemory(scanline);
  x_vector=(double *) RelinquishMagickMemory(x_vector);
  scale_image->type=image->type;
  if (status == MagickFalse)
    scale_image=DestroyImage(scale_image);
  return(scale_image);
}