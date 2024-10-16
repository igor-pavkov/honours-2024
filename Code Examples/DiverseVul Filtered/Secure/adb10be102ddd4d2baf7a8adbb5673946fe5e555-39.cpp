string simpleCompress(const string& elabel, const string& root)
{
  string label=elabel;
  // FIXME: this relies on the semi-canonical escaped output from getLabelFromContent
  if(strchr(label.c_str(), '\\')) {
    boost::replace_all(label, "\\.", ".");
    boost::replace_all(label, "\\032", " ");
    boost::replace_all(label, "\\\\", "\\");   
  }
  typedef vector<pair<unsigned int, unsigned int> > parts_t;
  parts_t parts;
  vstringtok(parts, label, ".");
  string ret;
  ret.reserve(label.size()+4);
  for(parts_t::const_iterator i=parts.begin(); i!=parts.end(); ++i) {
    if(!root.empty() && !strncasecmp(root.c_str(), label.c_str() + i->first, 1 + label.length() - i->first)) { // also match trailing 0, hence '1 +'
      const unsigned char rootptr[2]={0xc0,0x11};
      ret.append((const char *) rootptr, 2);
      return ret;
    }
    ret.append(1, (char)(i->second - i->first));
    ret.append(label.c_str() + i->first, i->second - i->first);
  }
  ret.append(1, (char)0);
  return ret;
}