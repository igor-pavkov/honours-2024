BOOLEAN sdb_set_breakpoint(const char *pp, int given_lineno)
{
  idhdl h=ggetid(pp);
  if ((h==NULL)||(IDTYP(h)!=PROC_CMD))
  {
    PrintS(" not found\n");
    return TRUE;
  }
  else
  {
    procinfov p=(procinfov)IDDATA(h);
    #ifdef HAVE_DYNAMIC_LOADING
    if (p->language!=LANG_SINGULAR)
    {
      PrintS("is not a Singular procedure\n");
      return TRUE;
    }
    #endif
    int lineno;
    if (given_lineno >0) lineno=given_lineno;
    else                 lineno=p->data.s.body_lineno;
    int i;
    if (given_lineno== -1)
    {
      i=p->trace_flag;
      p->trace_flag &=1;
      Print("breakpoints in %s deleted(%#x)\n",p->procname,i &255);
      return FALSE;
    }
    i=0;
    while((i<7) && (sdb_lines[i]!=-1)) i++;
    if (sdb_lines[i]!= -1)
    {
      PrintS("too many breakpoints set, max is 7\n");
      return TRUE;
    }
    sdb_lines[i]=lineno;
    sdb_files[i]=p->libname;
    i++;
    p->trace_flag|=(1<<i);
    Print("breakpoint %d, at line %d in %s\n",i,lineno,p->procname);
    return FALSE;
  }
}