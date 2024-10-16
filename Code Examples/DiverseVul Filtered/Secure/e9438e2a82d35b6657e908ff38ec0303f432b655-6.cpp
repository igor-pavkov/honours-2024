MagickExport MagickBooleanType ProfileImage(Image *image,const char *name,
  const void *datum,const size_t length,ExceptionInfo *exception)
{
#define ProfileImageTag  "Profile/Image"
#define ThrowProfileException(severity,tag,context) \
{ \
  if (source_profile != (cmsHPROFILE) NULL) \
    (void) cmsCloseProfile(source_profile); \
  if (target_profile != (cmsHPROFILE) NULL) \
    (void) cmsCloseProfile(target_profile); \
  ThrowBinaryException(severity,tag,context); \
}

  MagickBooleanType
    status;

  StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(name != (const char *) NULL);
  if ((datum == (const void *) NULL) || (length == 0))
    {
      char
        **arguments,
        *names;

      int
        number_arguments;

      register ssize_t
        i;

      /*
        Delete image profile(s).
      */
      names=ConstantString(name);
      (void) SubstituteString(&names,","," ");
      arguments=StringToArgv(names,&number_arguments);
      names=DestroyString(names);
      if (arguments == (char **) NULL)
        return(MagickTrue);
      ResetImageProfileIterator(image);
      for (name=GetNextImageProfile(image); name != (const char *) NULL; )
      {
        for (i=1; i < (ssize_t) number_arguments; i++)
        {
          if ((*arguments[i] == '!') &&
              (LocaleCompare(name,arguments[i]+1) == 0))
            break;
          if (GlobExpression(name,arguments[i],MagickTrue) != MagickFalse)
            {
              (void) DeleteImageProfile(image,name);
              break;
            }
        }
        name=GetNextImageProfile(image);
      }
      for (i=0; i < (ssize_t) number_arguments; i++)
        arguments[i]=DestroyString(arguments[i]);
      arguments=(char **) RelinquishMagickMemory(arguments);
      return(MagickTrue);
    }
  /*
    Add a ICC, IPTC, or generic profile to the image.
  */
  status=MagickTrue;
  profile=AcquireStringInfo((size_t) length);
  SetStringInfoDatum(profile,(unsigned char *) datum);
  if ((LocaleCompare(name,"icc") != 0) && (LocaleCompare(name,"icm") != 0))
    status=SetImageProfile(image,name,profile,exception);
  else
    {
      const StringInfo
        *icc_profile;

      icc_profile=GetImageProfile(image,"icc");
      if ((icc_profile != (const StringInfo *) NULL) &&
          (CompareStringInfo(icc_profile,profile) == 0))
        {
          const char
            *value;

          value=GetImageProperty(image,"exif:ColorSpace",exception);
          (void) value;
          /* Future.
          if (LocaleCompare(value,"1") != 0)
            (void) SetsRGBImageProfile(image,exception);
          value=GetImageProperty(image,"exif:InteroperabilityIndex",exception);
          if (LocaleCompare(value,"R98.") != 0)
            (void) SetsRGBImageProfile(image,exception);
          value=GetImageProperty(image,"exif:InteroperabilityIndex",exception);
          if (LocaleCompare(value,"R03.") != 0)
            (void) SetAdobeRGB1998ImageProfile(image,exception);
          */
          icc_profile=GetImageProfile(image,"icc");
        }
      if ((icc_profile != (const StringInfo *) NULL) &&
          (CompareStringInfo(icc_profile,profile) == 0))
        {
          profile=DestroyStringInfo(profile);
          return(MagickTrue);
        }
#if !defined(MAGICKCORE_LCMS_DELEGATE)
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn",
        "'%s' (LCMS)",image->filename);
#else
      {
        cmsHPROFILE
          source_profile;

        CMSExceptionInfo
          cms_exception;

        /*
          Transform pixel colors as defined by the color profiles.
        */
        cmsSetLogErrorHandler(CMSExceptionHandler);
        cms_exception.image=image;
        cms_exception.exception=exception;
        (void) cms_exception;
        source_profile=cmsOpenProfileFromMemTHR(&cms_exception,
          GetStringInfoDatum(profile),(cmsUInt32Number)
          GetStringInfoLength(profile));
        if (source_profile == (cmsHPROFILE) NULL)
          ThrowBinaryException(ResourceLimitError,
            "ColorspaceColorProfileMismatch",name);
        if ((cmsGetDeviceClass(source_profile) != cmsSigLinkClass) &&
            (icc_profile == (StringInfo *) NULL))
          status=SetImageProfile(image,name,profile,exception);
        else
          {
            CacheView
              *image_view;

            ColorspaceType
              source_colorspace,
              target_colorspace;

            cmsColorSpaceSignature
              signature;

            cmsHPROFILE
              target_profile;

            cmsHTRANSFORM
              *restrict transform;

            cmsUInt32Number
              flags,
              source_type,
              target_type;

            int
              intent;

            MagickBooleanType
              status;

            MagickOffsetType
              progress;

            size_t
              source_channels,
              target_channels;

            ssize_t
              y;

            unsigned short
              **restrict source_pixels,
              **restrict target_pixels;

            target_profile=(cmsHPROFILE) NULL;
            if (icc_profile != (StringInfo *) NULL)
              {
                target_profile=source_profile;
                source_profile=cmsOpenProfileFromMemTHR(&cms_exception,
                  GetStringInfoDatum(icc_profile),(cmsUInt32Number)
                  GetStringInfoLength(icc_profile));
                if (source_profile == (cmsHPROFILE) NULL)
                  ThrowProfileException(ResourceLimitError,
                    "ColorspaceColorProfileMismatch",name);
              }
            switch (cmsGetColorSpace(source_profile))
            {
              case cmsSigCmykData:
              {
                source_colorspace=CMYKColorspace;
                source_type=(cmsUInt32Number) TYPE_CMYK_16;
                source_channels=4;
                break;
              }
              case cmsSigGrayData:
              {
                source_colorspace=GRAYColorspace;
                source_type=(cmsUInt32Number) TYPE_GRAY_16;
                source_channels=1;
                break;
              }
              case cmsSigLabData:
              {
                source_colorspace=LabColorspace;
                source_type=(cmsUInt32Number) TYPE_Lab_16;
                source_channels=3;
                break;
              }
              case cmsSigLuvData:
              {
                source_colorspace=YUVColorspace;
                source_type=(cmsUInt32Number) TYPE_YUV_16;
                source_channels=3;
                break;
              }
              case cmsSigRgbData:
              {
                source_colorspace=sRGBColorspace;
                source_type=(cmsUInt32Number) TYPE_RGB_16;
                source_channels=3;
                break;
              }
              case cmsSigXYZData:
              {
                source_colorspace=XYZColorspace;
                source_type=(cmsUInt32Number) TYPE_XYZ_16;
                source_channels=3;
                break;
              }
              case cmsSigYCbCrData:
              {
                source_colorspace=YCbCrColorspace;
                source_type=(cmsUInt32Number) TYPE_YCbCr_16;
                source_channels=3;
                break;
              }
              default:
              {
                source_colorspace=UndefinedColorspace;
                source_type=(cmsUInt32Number) TYPE_RGB_16;
                source_channels=3;
                break;
              }
            }
            signature=cmsGetPCS(source_profile);
            if (target_profile != (cmsHPROFILE) NULL)
              signature=cmsGetColorSpace(target_profile);
            switch (signature)
            {
              case cmsSigCmykData:
              {
                target_colorspace=CMYKColorspace;
                target_type=(cmsUInt32Number) TYPE_CMYK_16;
                target_channels=4;
                break;
              }
              case cmsSigLabData:
              {
                target_colorspace=LabColorspace;
                target_type=(cmsUInt32Number) TYPE_Lab_16;
                target_channels=3;
                break;
              }
              case cmsSigGrayData:
              {
                target_colorspace=GRAYColorspace;
                target_type=(cmsUInt32Number) TYPE_GRAY_16;
                target_channels=1;
                break;
              }
              case cmsSigLuvData:
              {
                target_colorspace=YUVColorspace;
                target_type=(cmsUInt32Number) TYPE_YUV_16;
                target_channels=3;
                break;
              }
              case cmsSigRgbData:
              {
                target_colorspace=sRGBColorspace;
                target_type=(cmsUInt32Number) TYPE_RGB_16;
                target_channels=3;
                break;
              }
              case cmsSigXYZData:
              {
                target_colorspace=XYZColorspace;
                target_type=(cmsUInt32Number) TYPE_XYZ_16;
                target_channels=3;
                break;
              }
              case cmsSigYCbCrData:
              {
                target_colorspace=YCbCrColorspace;
                target_type=(cmsUInt32Number) TYPE_YCbCr_16;
                target_channels=3;
                break;
              }
              default:
              {
                target_colorspace=UndefinedColorspace;
                target_type=(cmsUInt32Number) TYPE_RGB_16;
                target_channels=3;
                break;
              }
            }
            if ((source_colorspace == UndefinedColorspace) ||
                (target_colorspace == UndefinedColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == GRAYColorspace) &&
                 (IsImageGray(image,exception) == MagickFalse))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == CMYKColorspace) &&
                 (image->colorspace != CMYKColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == XYZColorspace) &&
                 (image->colorspace != XYZColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace == YCbCrColorspace) &&
                 (image->colorspace != YCbCrColorspace))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
             if ((source_colorspace != CMYKColorspace) &&
                 (source_colorspace != LabColorspace) &&
                 (source_colorspace != XYZColorspace) &&
                 (source_colorspace != YCbCrColorspace) &&
                 (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse))
              ThrowProfileException(ImageError,"ColorspaceColorProfileMismatch",
                name);
            switch (image->rendering_intent)
            {
              case AbsoluteIntent: intent=INTENT_ABSOLUTE_COLORIMETRIC; break;
              case PerceptualIntent: intent=INTENT_PERCEPTUAL; break;
              case RelativeIntent: intent=INTENT_RELATIVE_COLORIMETRIC; break;
              case SaturationIntent: intent=INTENT_SATURATION; break;
              default: intent=INTENT_PERCEPTUAL; break;
            }
            flags=cmsFLAGS_HIGHRESPRECALC;
#if defined(cmsFLAGS_BLACKPOINTCOMPENSATION)
            if (image->black_point_compensation != MagickFalse)
              flags|=cmsFLAGS_BLACKPOINTCOMPENSATION;
#endif
            transform=AcquireTransformThreadSet(image,source_profile,
              source_type,target_profile,target_type,intent,flags);
            if (transform == (cmsHTRANSFORM *) NULL)
              ThrowProfileException(ImageError,"UnableToCreateColorTransform",
                name);
            /*
              Transform image as dictated by the source & target image profiles.
            */
            source_pixels=AcquirePixelThreadSet(image->columns,source_channels);
            target_pixels=AcquirePixelThreadSet(image->columns,target_channels);
            if ((source_pixels == (unsigned short **) NULL) ||
                (target_pixels == (unsigned short **) NULL))
              {
                transform=DestroyTransformThreadSet(transform);
                ThrowProfileException(ResourceLimitError,
                  "MemoryAllocationFailed",image->filename);
              }
            if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
              {
                target_pixels=DestroyPixelThreadSet(target_pixels);
                source_pixels=DestroyPixelThreadSet(source_pixels);
                transform=DestroyTransformThreadSet(transform);
                if (source_profile != (cmsHPROFILE) NULL)
                  (void) cmsCloseProfile(source_profile);
                if (target_profile != (cmsHPROFILE) NULL)
                  (void) cmsCloseProfile(target_profile);
                return(MagickFalse);
              }
            if (target_colorspace == CMYKColorspace)
              (void) SetImageColorspace(image,target_colorspace,exception);
            status=MagickTrue;
            progress=0;
            image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
            #pragma omp parallel for schedule(static,4) shared(status) \
              magick_threads(image,image,image->rows,1)
#endif
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const int
                id = GetOpenMPThreadId();

              MagickBooleanType
                sync;

              register ssize_t
                x;

              register Quantum
                *restrict q;

              register unsigned short
                *p;

              if (status == MagickFalse)
                continue;
              q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
                exception);
              if (q == (Quantum *) NULL)
                {
                  status=MagickFalse;
                  continue;
                }
              p=source_pixels[id];
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *p++=ScaleQuantumToShort(GetPixelRed(image,q));
                if (source_channels > 1)
                  {
                    *p++=ScaleQuantumToShort(GetPixelGreen(image,q));
                    *p++=ScaleQuantumToShort(GetPixelBlue(image,q));
                  }
                if (source_channels > 3)
                  *p++=ScaleQuantumToShort(GetPixelBlack(image,q));
                q+=GetPixelChannels(image);
              }
              cmsDoTransform(transform[id],source_pixels[id],target_pixels[id],
                (unsigned int) image->columns);
              p=target_pixels[id];
              q-=image->columns*GetPixelChannels(image);
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                if (target_channels == 1)
                  SetPixelGray(image,ScaleShortToQuantum(*p),q);
                else
                  SetPixelRed(image,ScaleShortToQuantum(*p),q);
                p++;
                if (target_channels > 1)
                  {
                    SetPixelGreen(image,ScaleShortToQuantum(*p),q);
                    p++;
                    SetPixelBlue(image,ScaleShortToQuantum(*p),q);
                    p++;
                  }
                if (target_channels > 3)
                  {
                    SetPixelBlack(image,ScaleShortToQuantum(*p),q);
                    p++;
                  }
                q+=GetPixelChannels(image);
              }
              sync=SyncCacheViewAuthenticPixels(image_view,exception);
              if (sync == MagickFalse)
                status=MagickFalse;
              if (image->progress_monitor != (MagickProgressMonitor) NULL)
                {
                  MagickBooleanType
                    proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
                  #pragma omp critical (MagickCore_ProfileImage)
#endif
                  proceed=SetImageProgress(image,ProfileImageTag,progress++,
                    image->rows);
                  if (proceed == MagickFalse)
                    status=MagickFalse;
                }
            }
            image_view=DestroyCacheView(image_view);
            (void) SetImageColorspace(image,target_colorspace,exception);
            switch (signature)
            {
              case cmsSigRgbData:
              {
                image->type=image->alpha_trait != BlendPixelTrait ?
                  TrueColorType : TrueColorMatteType;
                break;
              }
              case cmsSigCmykData:
              {
                image->type=image->alpha_trait != BlendPixelTrait ?
                  ColorSeparationType : ColorSeparationMatteType;
                break;
              }
              case cmsSigGrayData:
              {
                image->type=image->alpha_trait != BlendPixelTrait ?
                  GrayscaleType : GrayscaleMatteType;
                break;
              }
              default:
                break;
            }
            target_pixels=DestroyPixelThreadSet(target_pixels);
            source_pixels=DestroyPixelThreadSet(source_pixels);
            transform=DestroyTransformThreadSet(transform);
            if (cmsGetDeviceClass(source_profile) != cmsSigLinkClass)
              status=SetImageProfile(image,name,profile,exception);
            if (target_profile != (cmsHPROFILE) NULL)
              (void) cmsCloseProfile(target_profile);
          }
        (void) cmsCloseProfile(source_profile);
      }
#endif
    }
  profile=DestroyStringInfo(profile);
  return(status);
}