void __fastcall TSCPFileSystem::SCPError(const UnicodeString Message, bool Fatal)
{
  SCPSendError(Message, Fatal);
  throw EScpFileSkipped(NULL, Message);
}