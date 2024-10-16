UnicodeString __fastcall TSCPFileSystem::DelimitStr(UnicodeString Str)
{
  if (!Str.IsEmpty())
  {
    Str = ::DelimitStr(Str, L"\\`$\"");
    if (Str[1] == L'-') Str = L"./"+Str;
  }
  return Str;
}