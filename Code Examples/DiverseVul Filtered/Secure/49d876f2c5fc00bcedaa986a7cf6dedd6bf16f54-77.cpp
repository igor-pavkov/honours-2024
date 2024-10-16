void __fastcall TSCPFileSystem::SCPSendError(const UnicodeString Message, bool Fatal)
{
  unsigned char ErrorLevel = (char)(Fatal ? 2 : 1);
  FTerminal->LogEvent(FORMAT(L"Sending SCP error (%d) to remote side:",
    ((int)ErrorLevel)));
  FSecureShell->Send(&ErrorLevel, 1);
  // We don't send exact error message, because some unspecified
  // characters can terminate remote scp
  FSecureShell->SendLine(L"scp: error");
}