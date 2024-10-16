static void lsr_read_group_content(GF_LASeRCodec *lsr, GF_Node *elt, Bool skip_object_content)
{
	u32 i, count;
	if (lsr->last_error) return;

	if (!skip_object_content) lsr_read_object_content(lsr, (SVG_Element*)elt);


	/*node attributes are all parsed*/
	if (elt->sgprivate->tag!=TAG_SVG_script)
		gf_node_init(elt);

	GF_LSR_READ_INT(lsr, count, 1, "opt_group");
	if (count) {
		GF_ChildNodeItem *last = NULL;
		count = lsr_read_vluimsbf5(lsr, "occ0");
		for (i=0; i<count; i++) {
			GF_Node *n;
			if (lsr->last_error) return;
			n = lsr_read_scene_content_model(lsr, (SVG_Element*)elt);
			if (n) {
				gf_node_register(n, elt);
				gf_node_list_add_child_last(& ((SVG_Element*)elt)->children, n, &last);
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[LASeR] ############## end %s ###########\n", gf_node_get_class_name(n)));
			} else {
				/*either error or text content*/
			}
		}
	}

	if (elt->sgprivate->tag==TAG_SVG_script)
		gf_node_init(elt);
}