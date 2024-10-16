static GooString* Dirname(GooString* str){
  
  char *p=str->c_str();
  int len=str->getLength();
  for (int i=len-1;i>=0;i--)
    if (*(p+i)==SLASH) 
      return new GooString(p,i+1);
  return new GooString();
} 