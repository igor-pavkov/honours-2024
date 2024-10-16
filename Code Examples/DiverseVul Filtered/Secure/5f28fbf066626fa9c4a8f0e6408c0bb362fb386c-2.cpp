void sdb_show_bp()
{
  for(int i=0; i<7;i++)
    if (sdb_lines[i]!= -1)
      Print("Breakpoint %d: %s::%d\n",i+1,sdb_files[i],sdb_lines[i]);
}