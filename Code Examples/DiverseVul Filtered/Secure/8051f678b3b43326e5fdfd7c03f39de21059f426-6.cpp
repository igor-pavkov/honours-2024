FontInfo::FontInfo(FontInfo& f) {
  name = f.name ? f.name->copy() : nullptr;
  file = f.file ? f.file->copy() : nullptr;
  encoding = f.encoding ? f.encoding->copy() : nullptr;
  substituteName = f.substituteName ? f.substituteName->copy() : nullptr;
  type = f.type;
  emb = f.emb;
  subset = f.subset;
  hasToUnicode = f.hasToUnicode;
  fontRef = f.fontRef;
  embRef = f.embRef;
}