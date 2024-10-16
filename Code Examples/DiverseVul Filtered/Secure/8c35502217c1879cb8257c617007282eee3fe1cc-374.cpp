void Magick::Image::matteColor(const Color &matteColor_)
{
  modifyImage();

  if (matteColor_.isValid())
    {
      image()->matte_color=matteColor_;
      options()->matteColor(matteColor_);
    }
  else
    {
      // Set to default matte color
      Color tmpColor("#BDBDBD");
      image()->matte_color=tmpColor;
      options()->matteColor(tmpColor);
    }
}