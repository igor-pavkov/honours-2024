MagickExport char *GetEnvironmentValue(const char *name)
{
  const char
    *environment;

  environment=getenv(name);
  if (environment == (const char *) NULL)
    return((char *) NULL);
  return(ConstantString(environment));
}