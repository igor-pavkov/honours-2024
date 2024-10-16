static const char *PngColorTypeToString(const unsigned int color_type)
{
  const char
    *result = "Unknown";

  switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
      result = "Gray";
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      result = "Gray+Alpha";
      break;
    case PNG_COLOR_TYPE_PALETTE:
      result = "Palette";
      break;
    case PNG_COLOR_TYPE_RGB:
      result = "RGB";
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      result = "RGB+Alpha";
      break;
    }

  return result;
}