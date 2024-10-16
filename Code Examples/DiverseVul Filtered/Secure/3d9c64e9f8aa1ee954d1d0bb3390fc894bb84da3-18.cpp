DLLEXPORT tjscalingfactor *tjGetScalingFactors(int *numscalingfactors)
{
  if (numscalingfactors == NULL) {
    snprintf(errStr, JMSG_LENGTH_MAX,
             "tjGetScalingFactors(): Invalid argument");
    return NULL;
  }

  *numscalingfactors = NUMSF;
  return (tjscalingfactor *)sf;
}