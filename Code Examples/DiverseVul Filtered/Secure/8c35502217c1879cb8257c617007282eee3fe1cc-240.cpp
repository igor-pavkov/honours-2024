void Magick::Image::resolutionUnits(
  const Magick::ResolutionType resolutionUnits_)
{
  modifyImage();
  image()->units=resolutionUnits_;
  options()->resolutionUnits(resolutionUnits_);
}