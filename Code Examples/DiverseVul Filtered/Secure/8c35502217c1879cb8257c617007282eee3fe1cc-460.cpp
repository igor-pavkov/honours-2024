void Magick::Image::quantizeTreeDepth(const size_t treeDepth_)
{
  modifyImage();
  options()->quantizeTreeDepth(treeDepth_);
}