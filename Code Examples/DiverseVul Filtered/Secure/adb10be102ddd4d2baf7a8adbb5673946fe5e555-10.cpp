  string getZoneRepresentation() const
  {
    ostringstream str;
    str<<"\\# "<<(unsigned int)d_record.size()<<" ";
    char hex[4];
    for(size_t n=0; n<d_record.size(); ++n) {
      snprintf(hex,sizeof(hex)-1, "%02x", d_record.at(n));
      str << hex;
    }
    return str.str();
  }