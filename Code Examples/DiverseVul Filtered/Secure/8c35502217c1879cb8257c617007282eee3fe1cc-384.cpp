void Magick::Image::draw(const Magick::Drawable &drawable_)
{
  DrawingWand
    *wand;

  modifyImage();

  wand=AcquireDrawingWand(options()->drawInfo(),image());

  if(wand)
    {
      drawable_.operator()(wand);

      DrawRender(wand);

      ClonePPDrawException(wand);
      wand=DestroyDrawingWand(wand);
      ThrowPPDrawException(quiet());
    }
}