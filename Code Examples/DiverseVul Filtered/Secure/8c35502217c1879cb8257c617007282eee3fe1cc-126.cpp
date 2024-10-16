void Magick::Image::evaluate(const ChannelType channel_,const ssize_t x_,
  const ssize_t y_,const size_t columns_,const size_t rows_,
  const MagickEvaluateOperator operator_,const double rvalue_)
{
  RectangleInfo
    geometry;

  MagickCore::Image
    *cropImage;

  geometry.width = columns_;
  geometry.height = rows_;
  geometry.x = x_;
  geometry.y = y_;

  GetPPException;
  cropImage=CropImage(image(),&geometry,exceptionInfo);
  GetAndSetPPChannelMask(channel_);
  EvaluateImage(cropImage,operator_,rvalue_,exceptionInfo);
  RestorePPChannelMask;
  (void) CompositeImage(image(),cropImage,image()->alpha_trait == 
    BlendPixelTrait ? OverCompositeOp : CopyCompositeOp,MagickFalse,
    geometry.x,geometry.y,exceptionInfo );
  cropImage=DestroyImageList(cropImage);
  ThrowImageException;
}