static void lsr_read_group_content_post_init(GF_LASeRCodec *lsr, SVG_Element *elt, Bool skip_init)
{
	u32 i, count;
	if (lsr->last_error) return;
	lsr_read_object_content(lsr, elt);

	GF_LSR_READ_INT(lsr, count, 1, "opt_group");
	if (count) {
		GF_ChildNodeItem *last = NULL;
		count = lsr_read_vluimsbf5(lsr, "occ0");
		for (i=0; i<count; i++) {
			GF_Node *n;
			if (lsr->last_error) return;
			n = lsr_read_scene_content_model(lsr, elt);
			if (n) {
				gf_node_register(n, (GF_Node*)elt);
				gf_node_list_add_child_last(&elt->children, n, &last);
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CODING, ("[LASeR] ############## end %s ###########\n", gf_node_get_class_name(n)));
			} else {
				/*either error or text content*/
			}
		}
	}
	if (!skip_init) gf_node_init((GF_Node*)elt);
}