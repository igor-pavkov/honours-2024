static void ReorderSDP(char *sdp_text, Bool is_movie_sdp)
{
	char *cur;
	GF_List *lines = gf_list_new();
	cur = sdp_text;
	while (cur) {
		char b;
		char *st = strstr(cur, "\r\n");
		assert(st);
		st += 2;
		if (!st[0]) {
			AddSDPLine(lines, gf_strdup(cur), is_movie_sdp);
			break;
		}
		b = st[0];
		st[0] = 0;
		AddSDPLine(lines, gf_strdup(cur), is_movie_sdp);
		st[0] = b;
		cur = st;
	}
	strcpy(sdp_text, "");
	while (gf_list_count(lines)) {
		cur = (char *)gf_list_get(lines, 0);
		gf_list_rem(lines, 0);
		strcat(sdp_text, cur);
		gf_free(cur);
	}
	gf_list_del(lines);
}