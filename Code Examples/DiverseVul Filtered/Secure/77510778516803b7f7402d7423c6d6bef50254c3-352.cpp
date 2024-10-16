void vwid_box_del(GF_Box *s)
{
	u32 i;
	GF_ViewIdentifierBox *ptr = (GF_ViewIdentifierBox *) s;
	if (ptr->views) {
		for (i=0; i<ptr->num_views; i++) {
			if (ptr->views[i].view_refs)
				gf_free(ptr->views[i].view_refs);
		}
		gf_free(ptr->views);
	}
	gf_free(ptr);
}