void Magick::Image::writePixels(const Magick::QuantumType quantum_,
  unsigned char *destination_)
{
  QuantumInfo
    *quantum_info;

  quantum_info=AcquireQuantumInfo(imageInfo(),image());
  GetPPException;
  ExportQuantumPixels(image(),(MagickCore::CacheView *) NULL,quantum_info,
    quantum_,destination_, exceptionInfo);
  quantum_info=DestroyQuantumInfo(quantum_info);
  ThrowImageException;
}