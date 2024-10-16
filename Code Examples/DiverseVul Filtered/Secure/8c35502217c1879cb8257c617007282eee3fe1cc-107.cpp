void Magick::Image::annotate(const std::string &text_,
  const Geometry &boundingArea_,const GravityType gravity_,
  const double degrees_)
{
  AffineMatrix
    oaffine;

  char
    boundingArea[MagickPathExtent];

  DrawInfo
    *drawInfo;

  modifyImage();

  drawInfo=options()->drawInfo();
  drawInfo->text=DestroyString(drawInfo->text);
  drawInfo->text=const_cast<char *>(text_.c_str());
  drawInfo->geometry=DestroyString(drawInfo->geometry);

  if (boundingArea_.isValid())
    {
      if (boundingArea_.width() == 0 || boundingArea_.height() == 0)
        {
          FormatLocaleString(boundingArea,MagickPathExtent,"%+.20g%+.20g",
            (double) boundingArea_.xOff(),(double) boundingArea_.yOff());
        }
      else
        {
          (void) CopyMagickString(boundingArea,
            std::string(boundingArea_).c_str(), MagickPathExtent);
        }
      drawInfo->geometry=boundingArea;
    }

  drawInfo->gravity=gravity_;

  oaffine=drawInfo->affine;
  if (degrees_ != 0.0)
    {
       AffineMatrix
         affine,
         current;

       affine.sx=1.0;
       affine.rx=0.0;
       affine.ry=0.0;
       affine.sy=1.0;
       affine.tx=0.0;
       affine.ty=0.0;

       current=drawInfo->affine;
       affine.sx=cos(DegreesToRadians(fmod(degrees_,360.0)));
       affine.rx=sin(DegreesToRadians(fmod(degrees_,360.0)));
       affine.ry=(-sin(DegreesToRadians(fmod(degrees_,360.0))));
       affine.sy=cos(DegreesToRadians(fmod(degrees_,360.0)));

       drawInfo->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
       drawInfo->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
       drawInfo->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
       drawInfo->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
       drawInfo->affine.tx=current.sx*affine.tx+current.ry*affine.ty
         +current.tx;
    }

  GetPPException;
  AnnotateImage(image(),drawInfo,exceptionInfo);

  // Restore original values
  drawInfo->affine=oaffine;
  drawInfo->text=(char *) NULL;
  drawInfo->geometry=(char *) NULL;

  ThrowImageException;
}