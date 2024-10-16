static inline void FlattenPixelInfo(const Image *image,const PixelInfo *p,
  const double alpha,const Quantum *q,const double beta,
  Quantum *composite)
{
  double
    Da,
    gamma,
    Sa;

  register ssize_t
    i;

  /*
    Compose pixel p over pixel q with the given alpha.
  */
  Sa=QuantumScale*alpha;
  Da=QuantumScale*beta,
  gamma=Sa*(-Da)+Sa+Da;
  gamma=PerceptibleReciprocal(gamma);
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel = GetPixelChannelChannel(image,i);
    PixelTrait traits = GetPixelChannelTraits(image,channel);
    if (traits == UndefinedPixelTrait)
      continue;
    switch (channel)
    {
      case RedPixelChannel:
      {
        composite[i]=ClampToQuantum(gamma*MagickOver_((double) q[i],beta,
          (double) p->red,alpha));
        break;
      }
      case GreenPixelChannel:
      {
        composite[i]=ClampToQuantum(gamma*MagickOver_((double) q[i],beta,
          (double) p->green,alpha));
        break;
      }
      case BluePixelChannel:
      {
        composite[i]=ClampToQuantum(gamma*MagickOver_((double) q[i],beta,
          (double) p->blue,alpha));
        break;
      }
      case BlackPixelChannel:
      {
        composite[i]=ClampToQuantum(gamma*MagickOver_((double) q[i],beta,
          (double) p->black,alpha));
        break;
      }
      case AlphaPixelChannel:
      {
        composite[i]=ClampToQuantum(QuantumRange*(Sa*(-Da)+Sa+Da));
        break;
      }
      default:
        break;
    }
  }
}