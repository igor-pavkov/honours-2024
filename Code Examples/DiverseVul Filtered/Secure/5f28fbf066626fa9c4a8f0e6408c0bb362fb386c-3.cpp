int sdb_checkline(char f)
{
  int i;
  char ff=f>>1;
  for(i=0;i<7;i++)
  {
    if((ff & 1) && (yylineno==sdb_lines[i]))
      return i+1;
    ff>>=1;
    if (ff==0) return 0;
  }
  return 0;
}