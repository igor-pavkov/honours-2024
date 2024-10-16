void Magick::Image::fontTypeMetricsMultiline(const std::string &text_,
  TypeMetric *metrics)
{
  DrawInfo
    *drawInfo;

  drawInfo=options()->drawInfo();
  drawInfo->text=const_cast<char *>(text_.c_str());
  GetPPException;
  GetMultilineTypeMetrics(image(),drawInfo,&(metrics->_typeMetric),exceptionInfo);
  drawInfo->text=0;
  ThrowImageException;
}