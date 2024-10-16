void __fastcall TSCPFileSystem::ChangeDirectory(const UnicodeString Directory)
{
  UnicodeString ToDir;
  if (!Directory.IsEmpty() &&
      ((Directory[1] != L'~') || (Directory.SubString(1, 2) == L"~ ")))
  {
    ToDir = L"\"" + DelimitStr(Directory) + L"\"";
  }
  else
  {
    ToDir = DelimitStr(Directory);
  }
  ExecCommand(fsChangeDirectory, ARRAYOFCONST((ToDir)));
  FCachedDirectoryChange = L"";
}