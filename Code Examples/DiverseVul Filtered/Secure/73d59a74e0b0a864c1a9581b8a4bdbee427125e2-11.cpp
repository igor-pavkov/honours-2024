ModuleExport MagickBooleanType ReadPSDLayers(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,ExceptionInfo *exception)
{
  PolicyDomain
    domain;

  PolicyRights
    rights;

  domain=CoderPolicyDomain;
  rights=ReadPolicyRights;
  if (IsRightsAuthorized(domain,rights,"PSD") == MagickFalse)
    return(MagickTrue);
  return(ReadPSDLayersInternal(image,image_info,psd_info,MagickFalse,
    exception));
}