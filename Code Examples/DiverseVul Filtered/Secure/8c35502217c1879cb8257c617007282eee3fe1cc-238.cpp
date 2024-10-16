void Magick::Image::draw(const std::vector<Magick::Drawable> &drawable_)
{
  DrawingWand
    *wand;

  modifyImage();

  wand= AcquireDrawingWand(options()->drawInfo(),image());

  if(wand)
    {
      for (std::vector<Magick::Drawable>::const_iterator p = drawable_.begin();
           p != drawable_.end(); p++ )
        {
          p->operator()(wand);
          if (DrawGetExceptionType(wand) != MagickCore::UndefinedException)
            break;
        }

      if (DrawGetExceptionType(wand) == MagickCore::UndefinedException)
        DrawRender(wand);

      ClonePPDrawException(wand);
      wand=DestroyDrawingWand(wand);
      ThrowPPDrawException(quiet());
    }
}