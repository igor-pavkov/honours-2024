static string txtEscape(const string &name)
{
  string ret;
  char ebuf[5];

  for(string::const_iterator i=name.begin();i!=name.end();++i) {
    if((unsigned char) *i > 127 || (unsigned char) *i < 32) {
      snprintf(ebuf, sizeof(ebuf), "\\%03u", (unsigned char)*i);
      ret += ebuf;
    }
    else if(*i=='"' || *i=='\\'){
      ret += '\\';
      ret += *i;
    }
    else
      ret += *i;
  }
  return ret;
}