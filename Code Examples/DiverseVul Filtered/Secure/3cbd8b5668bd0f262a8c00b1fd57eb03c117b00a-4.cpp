static int icon_cb(void *ptr, uint32_t type, uint32_t name, uint32_t lang, uint32_t rva) {
    struct ICONS *icons = ptr;
    type = type; lang = lang;
    cli_dbgmsg("icon_cb: got icon %x\n", name);
    if(icons->cnt >= 100)
	return 1;
    icons->rvas[icons->cnt] = rva;
    icons->cnt++;
    return 0;
}