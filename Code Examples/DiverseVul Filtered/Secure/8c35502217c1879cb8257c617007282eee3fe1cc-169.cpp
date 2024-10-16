void Magick::Image::affineTransform(const DrawableAffine &affine_)
{
  AffineMatrix
    _affine;

  MagickCore::Image
    *newImage;

  _affine.sx=affine_.sx();
  _affine.sy=affine_.sy();
  _affine.rx=affine_.rx();
  _affine.ry=affine_.ry();
  _affine.tx=affine_.tx();
  _affine.ty=affine_.ty();

  GetPPException;
  newImage=AffineTransformImage(constImage(),&_affine,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}