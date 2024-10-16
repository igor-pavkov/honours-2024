bool isRootReferral(const MOADNSParser::answers_t& answers)
{
  if(answers.empty())
    return false;

  bool ok=true;
  for(MOADNSParser::answers_t::const_iterator iter = answers.begin(); iter != answers.end(); ++iter) {
    //    cerr<<(int)iter->first.d_place<<", "<<iter->first.d_name<<" "<<iter->first.d_type<<", # "<<answers.size()<<endl;
    if(iter->first.d_place!=2)
      ok=false;
    if(!iter->first.d_name.isRoot() || iter->first.d_type!=QType::NS)
      ok=false;
  }
  return ok;
}