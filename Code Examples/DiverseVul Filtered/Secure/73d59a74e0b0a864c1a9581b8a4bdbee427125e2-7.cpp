static const char *CompositeOperatorToPSDBlendMode(Image *image)
{
  switch (image->compose)
  {
    case ColorBurnCompositeOp:
      return(image->endian == LSBEndian ? "vidi" : "idiv");
    case ColorDodgeCompositeOp:
      return(image->endian == LSBEndian ? " vid" : "div ");
    case ColorizeCompositeOp:
      return(image->endian == LSBEndian ? "rloc" : "colr");
    case DarkenCompositeOp:
      return(image->endian == LSBEndian ? "krad" : "dark");
    case DifferenceCompositeOp:
      return(image->endian == LSBEndian ? "ffid" : "diff");
    case DissolveCompositeOp:
      return(image->endian == LSBEndian ? "ssid" : "diss");
    case ExclusionCompositeOp:
      return(image->endian == LSBEndian ? "dums" : "smud");
    case HardLightCompositeOp:
      return(image->endian == LSBEndian ? "tiLh" : "hLit");
    case HardMixCompositeOp:
      return(image->endian == LSBEndian ? "xiMh" : "hMix");
    case HueCompositeOp:
      return(image->endian == LSBEndian ? " euh" : "hue ");
    case LightenCompositeOp:
      return(image->endian == LSBEndian ? "etil" : "lite");
    case LinearBurnCompositeOp:
      return(image->endian == LSBEndian ? "nrbl" : "lbrn");
    case LinearDodgeCompositeOp:
      return(image->endian == LSBEndian ? "gddl" : "lddg");
    case LinearLightCompositeOp:
      return(image->endian == LSBEndian ? "tiLl" : "lLit");
    case LuminizeCompositeOp:
      return(image->endian == LSBEndian ? " mul" : "lum ");
    case MultiplyCompositeOp:
      return(image->endian == LSBEndian ? " lum" : "mul ");
    case OverlayCompositeOp:
      return(image->endian == LSBEndian ? "revo" : "over");
    case PinLightCompositeOp:
      return(image->endian == LSBEndian ? "tiLp" : "pLit");
    case SaturateCompositeOp:
      return(image->endian == LSBEndian ? " tas" : "sat ");
    case ScreenCompositeOp:
      return(image->endian == LSBEndian ? "nrcs" : "scrn");
    case SoftLightCompositeOp:
      return(image->endian == LSBEndian ? "tiLs" : "sLit");
    case VividLightCompositeOp:
      return(image->endian == LSBEndian ? "tiLv" : "vLit");
    case OverCompositeOp:
    default:
      return(image->endian == LSBEndian ? "mron" : "norm");
  }
}