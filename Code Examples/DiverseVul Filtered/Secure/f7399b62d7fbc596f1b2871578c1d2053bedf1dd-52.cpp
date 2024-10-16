void ImageOverlay::get_background_color(uint16_t col[4]) const
{
  for (int i=0;i<4;i++) {
    col[i] = m_background_color[i];
  }
}