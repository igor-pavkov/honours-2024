ModuleExport MagickBooleanType WritePSDLayers(Image * image,
  const ImageInfo *image_info,const PSDInfo *psd_info,ExceptionInfo *exception)
{
  PolicyDomain
    domain;

  PolicyRights
    rights;

  domain=CoderPolicyDomain;
  rights=WritePolicyRights;
  if (IsRightsAuthorized(domain,rights,"PSD") == MagickFalse)
    return(MagickTrue);
  return WritePSDLayersInternal(image,image_info,psd_info,(size_t*) NULL,
    exception);
}