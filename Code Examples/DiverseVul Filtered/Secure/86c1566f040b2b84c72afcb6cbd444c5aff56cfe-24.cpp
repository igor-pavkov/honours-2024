static void AddSDPLine(GF_List *list, char *sdp_text, Bool is_movie_sdp)
{
	const char *sdp_order;
	u32 i, count = gf_list_count(list);
	char fc = sdp_text[0];

	sdp_order = (is_movie_sdp) ? "vosiuepcbzkatr" : "micbka";
	for (i=0; i<count; i++) {
		char *l = (char *)gf_list_get(list, i);
		char *s1 = (char *)strchr(sdp_order, l[0]);
		char *s2 = (char *)strchr(sdp_order, fc);
		if (s1 && s2 && (strlen(s2)>strlen(s1))) {
			gf_list_insert(list, sdp_text, i);
			return;
		}
	}
	gf_list_add(list, sdp_text);
}