void simpleExpandTo(const string& label, unsigned int frompos, string& ret)
{
  unsigned int labellen=0;
  while((labellen=(unsigned char)label.at(frompos++))) {
    ret.append(label.c_str()+frompos, labellen);
    ret.append(1,'.');
    frompos+=labellen;
  }
}