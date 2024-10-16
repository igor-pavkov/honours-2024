static const char *strrstr( const char *s, const char *ss )
{
  const char *p = strstr( s, ss );
  for( const char *pp = p; pp != nullptr; pp = strstr( p+1, ss ) ){
    p = pp;
  }
  return p;
}