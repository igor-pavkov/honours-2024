void PrintBuiltInBoxes(Bool do_cov)
{
	u32 i, count=gf_isom_get_num_supported_boxes();
	fprintf(stdout, "<Boxes>\n");
	//index 0 is our internal unknown box handler
	for (i=1; i<count; i++) {
		gf_isom_dump_supported_box(i, stdout);
        if (do_cov) {
			u32 btype = gf_isom_get_supported_box_type(i);
            GF_Box *b=gf_isom_box_new(btype);
            if (b) {
                GF_Box *c=NULL;
                gf_isom_clone_box(b, &c);
                if (c) gf_isom_box_del(c);
                gf_isom_box_del(b);
            }
        }
	}
	fprintf(stdout, "</Boxes>\n");
}