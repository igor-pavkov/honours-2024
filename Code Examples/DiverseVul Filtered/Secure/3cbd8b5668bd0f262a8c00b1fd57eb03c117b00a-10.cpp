void cli_icongroupset_add(const char *groupname, icon_groupset *set, unsigned int type, cli_ctx *ctx) {
    struct icon_matcher *matcher;
    unsigned int i, j;

    if(type>1 || !ctx || !ctx->engine || !(matcher = ctx->engine->iconcheck) || !matcher->group_counts[type])
	return;

    j = matcher->group_counts[type];
    if(groupname[0] == '*' && !groupname[1]) {
	set->v[type][0] = set->v[type][1] = set->v[type][2] = set->v[type][3] = ~0;
	return;
    }
    for(i=0; i<j; i++) {
	if(!strcmp(groupname, matcher->group_names[type][i]))
	    break;
    }
    if(i == j)
	cli_dbgmsg("cli_icongroupset_add: failed to locate icon group%u %s\n", type, groupname);
    else {
	j = i % 64;
	i /= 64;
	set->v[type][i] |= (uint64_t)1<<j;
    }
}