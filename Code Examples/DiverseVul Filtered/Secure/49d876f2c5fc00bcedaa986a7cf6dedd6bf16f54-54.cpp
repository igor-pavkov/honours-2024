void __fastcall TSCPFileSystem::CreateLink(const UnicodeString FileName,
  const UnicodeString PointTo, bool Symbolic)
{
  ExecCommand(fsCreateLink,
    ARRAYOFCONST((Symbolic ? L"-s" : L"", DelimitStr(PointTo), DelimitStr(FileName))));
}