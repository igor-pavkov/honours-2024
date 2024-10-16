static inline MagickBooleanType IsFloatDefined(const float value)
{
  union
  {
    unsigned int
      unsigned_value;

    float
      float_value;
  } quantum;

  quantum.unsigned_value=0U;
  quantum.float_value=(float) value;
  if (quantum.unsigned_value == 0U)
    return(MagickFalse);
  return(MagickTrue);
}