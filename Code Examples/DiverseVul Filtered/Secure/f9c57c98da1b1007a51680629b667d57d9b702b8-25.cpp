const struct timeval operator*(float fact, const struct timeval& rhs)
{
  //  cout<<"In: "<<rhs.tv_sec<<" + "<<rhs.tv_usec<<"\n";
  struct timeval ret;
  if( (2000000000 / fact) < rhs.tv_usec) {
    double d=1.0 * rhs.tv_usec * fact;
    ret.tv_sec=fact * rhs.tv_sec;
    ret.tv_sec+=(int) (d/1000000);
    d/=1000000;
    d-=(int)d;

    ret.tv_usec=(unsigned int)(1000000*d);
    normalizeTV(ret);
    
    cout<<"out complex: "<<ret.tv_sec<<" + "<<ret.tv_usec<<"\n";
    
    return ret;
  }

  ret.tv_sec=rhs.tv_sec * fact;
  ret.tv_usec=rhs.tv_usec * fact;

  normalizeTV(ret);
  //  cout<<"out simple: "<<ret.tv_sec<<" + "<<ret.tv_usec<<"\n";
  return ret;
}