void CLASS derror()
{
  if (!data_error) {
    dcraw_message (DCRAW_WARNING, "%s: ", ifname_display);
    if (feof(ifp))
      dcraw_message (DCRAW_WARNING,_("Unexpected end of file\n"));
    else
#ifdef HAVE_FSEEKO
      dcraw_message (DCRAW_WARNING,_("Corrupt data near 0x%llx\n"),
		(INT64) ftello(ifp));
#else
      dcraw_message (DCRAW_WARNING,_("Corrupt data near 0x%lx\n"), ftell(ifp));
#endif
  }
  data_error++;
}