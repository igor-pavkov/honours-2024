GooString *HtmlOutputDev::createImageFileName(const char *ext)
{
  return GooString::format("{0:s}-{1:d}_{2:d}.{3:s}", Docname->c_str(), pageNum, pages->getNumImages() + 1, ext);
}