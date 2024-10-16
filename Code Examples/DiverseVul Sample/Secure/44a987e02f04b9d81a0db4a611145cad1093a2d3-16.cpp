bool pch_rename (void)
{
  return p_rename[OLD] && p_rename[NEW]
	 && p_name[OLD] && p_name[NEW];
}