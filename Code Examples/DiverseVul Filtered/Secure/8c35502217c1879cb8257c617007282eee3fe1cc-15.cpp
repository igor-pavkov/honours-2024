void Magick::Image::addNoise(const NoiseType noiseType_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=AddNoiseImage(constImage(),noiseType_,1.0,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}