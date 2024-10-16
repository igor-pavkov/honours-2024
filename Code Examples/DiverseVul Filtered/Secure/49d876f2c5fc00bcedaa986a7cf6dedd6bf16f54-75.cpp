void __fastcall TSCPFileSystem::CreateDirectory(const UnicodeString & DirName, bool /*Encrypt*/)
{
  ExecCommand(fsCreateDirectory, ARRAYOFCONST((DelimitStr(DirName))));
}