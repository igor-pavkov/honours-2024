void sdb(Voice * currentVoice, const char * currLine, int len)
{
  int bp=0;
  if ((len>1)
  && ((currentVoice->pi->trace_flag & 1)
    || (bp=sdb_checkline(currentVoice->pi->trace_flag)))
  )
  {
    loop
    {
      char gdb[80];
      char *p=(char *)currLine+len-1;
      while ((*p<=' ') && (p!=currLine))
      {
        p--; len--;
      }
      if (p==currLine) return;

      currentVoice->pi->trace_flag&= ~1; // delete flag for "all lines"
      Print("(%s,%d) >>",currentVoice->filename,yylineno);
      fwrite(currLine,1,len,stdout);
      Print("<<\nbreakpoint %d (press ? for list of commands)\n",bp);
      p=fe_fgets_stdin(">>",gdb,80);
      while (*p==' ') p++;
      if (*p >' ')
      {
        sdb_lastcmd=*p;
      }
      Print("command:%c\n",sdb_lastcmd);
      switch(sdb_lastcmd)
      {
        case '?':
        case 'h':
        {
          PrintS(
          "b - print backtrace of calling stack\n"
          "B <proc> [<line>] - define breakpoint\n"
          "c - continue\n"
          "d - delete current breakpoint\n"
          "D - show all breakpoints\n"
          "e - edit the current procedure (current call will be aborted)\n"
          "h,? - display this help screen\n"
          "n - execute current line, break at next line\n"
          "p <var> - display type and value of the variable <var>\n"
          "q <flags> - quit debugger, set debugger flags(0,1,2)\n"
	  "   0: stop debug, 1:continue, 2: throw an error, return to toplevel\n"
          "Q - quit Singular\n");
          int i;
          for(i=0;i<7;i++)
          {
            if (sdb_lines[i] != -1)
              Print("breakpoint %d at line %d in %s\n",
                i,sdb_lines[i],sdb_files[i]);
          }
          break;
        }
        case 'd':
        {
          Print("delete break point %d\n",bp);
          currentVoice->pi->trace_flag &= (~Sy_bit(bp));
          if (bp!=0)
          {
            sdb_lines[bp-1]=-1;
          }
          break;
        }
        case 'D':
          sdb_show_bp();
          break;
	#if 0
	case 'l':
	{
	  extern void listall(int showproc);
	  listall(FALSE);
	  break;
	}
	#endif
        case 'n':
          currentVoice->pi->trace_flag|= 1;
          return;
        case 'e':
        {
          sdb_edit(currentVoice->pi);
          sdb_flags=2;
          return;
        }
        case 'p':
        {
          p=sdb_find_arg(p);
	  EXTERN_VAR int myynest;
          Print("variable `%s`at level %d",p,myynest);
          idhdl h=ggetid(p);
          if (h==NULL)
            PrintS(" not found\n");
          else
          {
            sleftv tmp;
            memset(&tmp,0,sizeof(tmp));
            tmp.rtyp=IDHDL;
            tmp.data=h;
            Print("(type %s):\n",Tok2Cmdname(tmp.Typ()));
            tmp.Print();
          }
          break;
        }
        case 'b':
          VoiceBackTrack();
          break;
        case 'B':
        {
          p=sdb_find_arg(p);
          Print("procedure `%s` ",p);
          sdb_set_breakpoint(p);
          break;
        }
        case 'q':
        {
          p=sdb_find_arg(p);
          if (*p!='\0')
          {
            sdb_flags=atoi(p);
            Print("new sdb_flags:%d\n",sdb_flags);
          }
          return;
        }
        case 'Q':
          m2_end(999);
        case 'c':
        default:
          return;
      }
    }
  }
}