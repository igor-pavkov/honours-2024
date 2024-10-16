static int groupicon_cb(void *ptr, uint32_t type, uint32_t name, uint32_t lang, uint32_t rva) {
    struct GICONS *gicons = ptr;
    type = type; lang = lang;
    cli_dbgmsg("groupicon_cb: got group %x\n", name);
    if(!gicons->cnt || gicons->lastg == name) {
	gicons->rvas[gicons->cnt] = rva;
	gicons->cnt++;
	gicons->lastg = name;
	if(gicons->cnt < 100)
	    return 0;
    }
    return 1;
}