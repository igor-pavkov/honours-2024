MagickExport TokenInfo *AcquireTokenInfo(void)
{
  TokenInfo
    *token_info;

  token_info=(TokenInfo *) AcquireMagickMemory(sizeof(*token_info));
  if (token_info == (TokenInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  token_info->signature=MagickCoreSignature;
  return(token_info);
}