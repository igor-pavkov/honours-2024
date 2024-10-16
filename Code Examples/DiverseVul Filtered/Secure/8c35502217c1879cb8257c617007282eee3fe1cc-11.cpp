void Magick::Image::classType(const ClassType class_)
{
  if (classType() == PseudoClass && class_ == DirectClass)
    {
      // Use SyncImage to synchronize the DirectClass pixels with the
      // color map and then set to DirectClass type.
      modifyImage();
      GetPPException;
      SyncImage(image(),exceptionInfo);
      ThrowImageException;
      image()->colormap=(PixelInfo *)RelinquishMagickMemory(image()->colormap);
      image()->storage_class=static_cast<MagickCore::ClassType>(DirectClass);
      return;
    }

  if (classType() == DirectClass && class_ == PseudoClass)
    {
      // Quantize to create PseudoClass color map
      modifyImage();
      quantizeColors(MaxColormapSize);
      quantize();
      image()->storage_class=static_cast<MagickCore::ClassType>(PseudoClass);
    }
}