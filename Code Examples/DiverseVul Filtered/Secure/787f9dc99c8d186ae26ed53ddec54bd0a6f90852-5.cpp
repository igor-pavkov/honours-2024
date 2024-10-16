MagickExport TokenInfo *DestroyTokenInfo(TokenInfo *token_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(token_info != (TokenInfo *) NULL);
  assert(token_info->signature == MagickCoreSignature);
  token_info->signature=(~MagickCoreSignature);
  token_info=(TokenInfo *) RelinquishMagickMemory(token_info);
  return(token_info);
}