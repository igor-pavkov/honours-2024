static void Riemersma(Image *image,CacheView *image_view,CubeInfo *cube_info,
  const size_t level,const unsigned int direction,ExceptionInfo *exception)
{
  if (level == 1)
    switch (direction)
    {
      case WestGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        break;
      }
      case EastGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        break;
      }
      case NorthGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        break;
      }
      case SouthGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        break;
      }
      default:
        break;
    }
  else
    switch (direction)
    {
      case WestGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,NorthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,WestGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,WestGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity,
          exception);
        break;
      }
      case EastGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,SouthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,EastGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,EastGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity,
          exception);
        break;
      }
      case NorthGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,WestGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,EastGravity,
          exception);
        break;
      }
      case SouthGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,EastGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity,
          exception);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity,
          exception);
        Riemersma(image,image_view,cube_info,level-1,WestGravity,
          exception);
        break;
      }
      default:
        break;
    }
}