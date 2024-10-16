string PacketReader::getText(bool multi)
{
  string ret;
  ret.reserve(40);
  while(d_pos < d_startrecordpos + d_recordlen ) {
    if(!ret.empty()) {
      ret.append(1,' ');
    }
    unsigned char labellen=d_content.at(d_pos++);
    
    ret.append(1,'"');
    if(labellen) { // no need to do anything for an empty string
      string val(&d_content.at(d_pos), &d_content.at(d_pos+labellen-1)+1);
      ret.append(txtEscape(val)); // the end is one beyond the packet
    }
    ret.append(1,'"');
    d_pos+=labellen;
    if(!multi)
      break;
  }

  return ret;
}