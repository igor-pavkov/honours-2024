MagickExport void GravityAdjustGeometry(const size_t width,
  const size_t height,const GravityType gravity,RectangleInfo *region)
{
  if (region->height == 0)
    region->height=height;
  if (region->width == 0)
    region->width=width;
  switch (gravity)
  {
    case NorthEastGravity:
    case EastGravity:
    case SouthEastGravity:
    {
      region->x=(ssize_t) (width-region->width-region->x);
      break;
    }
    case NorthGravity:
    case SouthGravity:
    case CenterGravity:
    {
      region->x+=(ssize_t) (width/2-region->width/2);
      break;
    }
    case ForgetGravity:
    case NorthWestGravity:
    case WestGravity:
    case SouthWestGravity:
    default:
      break;
  }
  switch (gravity)
  {
    case SouthWestGravity:
    case SouthGravity:
    case SouthEastGravity:
    {
      region->y=(ssize_t) (height-region->height-region->y);
      break;
    }
    case EastGravity:
    case WestGravity:
    case CenterGravity:
    {
      region->y+=(ssize_t) (height/2-region->height/2);
      break;
    }
    case ForgetGravity:
    case NorthWestGravity:
    case NorthGravity:
    case NorthEastGravity:
    default:
      break;
  }
  return;
}