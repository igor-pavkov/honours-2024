static Bool lsr_init_smil_times(GF_LASeRCodec *lsr, SVG_Element *anim, GF_List *times, SVG_Element *parent)
{
	u32 i, count;
	count = gf_list_count(times);
	for (i=0; i<count; i++) {
		SMIL_Time *t = (SMIL_Time *)gf_list_get(times, i);
		if (t->type==GF_SMIL_TIME_EVENT) {
			if (t->element_id) {
				if (t->element_id[0]=='N') {
					t->element = gf_sg_find_node(lsr->sg, atoi(t->element_id+1) + 1);
				} else {
					t->element = gf_sg_find_node_by_name(lsr->sg, t->element_id);
				}
				if (!t->element) return GF_FALSE;
				gf_free(t->element_id);
				t->element_id = NULL;
			}
			else if (!t->element) {
				if (t->event.parameter && (t->event.type==GF_EVENT_KEYDOWN) ) {
					t->element = lsr->sg->RootNode ? lsr->sg->RootNode : lsr->current_root;
				} else {
					t->element = (GF_Node*)parent;
				}
			}
		}
	}
	return GF_TRUE;
}