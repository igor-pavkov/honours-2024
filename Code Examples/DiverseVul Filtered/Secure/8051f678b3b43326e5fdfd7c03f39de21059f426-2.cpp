FontInfo::~FontInfo() {
  delete name;
  delete file;
  delete encoding;
  if (substituteName)
    delete substituteName;
}