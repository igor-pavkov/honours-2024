__fastcall TSCPFileSystem::TSCPFileSystem(TTerminal * ATerminal, TSecureShell * SecureShell):
  TCustomFileSystem(ATerminal)
{
  FSecureShell = SecureShell;
  FCommandSet = new TCommandSet(FTerminal->SessionData);
  FLsFullTime = FTerminal->SessionData->SCPLsFullTime;
  FOutput = new TStringList();
  FProcessingCommand = false;
  FOnCaptureOutput = NULL;

  FFileSystemInfo.ProtocolBaseName = L"SCP";
  FFileSystemInfo.ProtocolName = FFileSystemInfo.ProtocolBaseName;
}