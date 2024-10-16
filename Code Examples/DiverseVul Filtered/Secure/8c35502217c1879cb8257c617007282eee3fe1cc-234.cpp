void Magick::Image::readPixels(const Magick::QuantumType quantum_,
  const unsigned char *source_)
{
  QuantumInfo
    *quantum_info;

  quantum_info=AcquireQuantumInfo(imageInfo(),image());
  GetPPException;
  ImportQuantumPixels(image(),(MagickCore::CacheView *) NULL,quantum_info,
    quantum_,source_,exceptionInfo);
  quantum_info=DestroyQuantumInfo(quantum_info);
  ThrowImageException;
}