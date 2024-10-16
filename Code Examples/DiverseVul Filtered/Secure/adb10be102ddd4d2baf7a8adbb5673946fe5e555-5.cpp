static const string EncodeDNSLabel(const string& input)
{  
  if(input.length() == 1 && input[0]=='.') // otherwise we encode .. (long story)
    return string (1, 0);
    
  labelparts_t parts;
  bool unescapedSomething = labeltokUnescape(parts, input);
  string ret;

  if(!unescapedSomething) {
    for(labelparts_t::const_iterator i=parts.begin(); i!=parts.end(); ++i) {
      ret.append(1, i->second - i->first);
      ret.append(input.c_str() + i->first, i->second - i->first);
    }

  } else {
    for(labelparts_t::const_iterator i=parts.begin(); i!=parts.end(); ++i) {
      string part(input.c_str() + i->first, i->second - i->first);
      boost::replace_all(part, "\\\\", "\\");
      boost::replace_all(part, "\\.", ".");
    
      ret.append(1, part.length());
      ret.append(part);
    }  
  }    
  ret.append(1, 0);
  return ret;
}