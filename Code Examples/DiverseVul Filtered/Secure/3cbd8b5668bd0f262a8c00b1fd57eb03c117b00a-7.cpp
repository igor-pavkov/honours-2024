int cli_scanicon(icon_groupset *set, uint32_t resdir_rva, cli_ctx *ctx, struct cli_exe_section *exe_sections, uint16_t nsections, uint32_t hdr_size) {
    struct GICONS gicons;
    struct ICONS icons;
    unsigned int curicon, err;
    fmap_t *map = *ctx->fmap;

    gicons.cnt = 0;
    icons.cnt = 0;
    findres(14, 0xffffffff, resdir_rva, map, exe_sections, nsections, hdr_size, groupicon_cb, &gicons);
	
    for(curicon=0; curicon<gicons.cnt; curicon++) {
	uint8_t *grp = fmap_need_off_once(map, cli_rawaddr(gicons.rvas[curicon], exe_sections, nsections, &err, map->len, hdr_size), 16);
	if(grp && !err) {
	    uint32_t gsz = cli_readint32(grp + 4);
	    if(gsz>6) {
		uint32_t icnt;
		struct icondir {
		    uint8_t w;
		    uint8_t h;
		    uint8_t palcnt;
		    uint8_t rsvd;
		    uint16_t planes;
		    uint16_t depth;
		    uint32_t sz;
		    uint16_t id;
		} *dir;

		grp = fmap_need_off_once(map, cli_rawaddr(cli_readint32(grp), exe_sections, nsections, &err, map->len, hdr_size), gsz);
		if(grp && !err) {
		    icnt = cli_readint32(grp+2) >> 16;
		    grp+=6;
		    gsz-=6;

		    while(icnt && gsz >= 14) {
			dir = (struct icondir *)grp;
			cli_dbgmsg("cli_scanicon: Icongrp @%x - %ux%ux%u - (id=%x, rsvd=%u, planes=%u, palcnt=%u, sz=%x)\n", gicons.rvas[curicon], dir->w, dir->h, cli_readint16(&dir->depth), cli_readint16(&dir->id), cli_readint16(&dir->planes), dir->palcnt, dir->rsvd, cli_readint32(&dir->sz));
			findres(3, cli_readint16(&dir->id), resdir_rva, map, exe_sections, nsections, hdr_size, icon_cb, &icons);
			grp += 14;
			gsz -= 14;
		    }
		}
	    }
	}
    }

    for(curicon=0; curicon<icons.cnt; curicon++) {
	if(parseicon(set, icons.rvas[curicon], ctx, exe_sections, nsections, hdr_size) == CL_VIRUS)
	    return CL_VIRUS;
    }
    return 0;
}