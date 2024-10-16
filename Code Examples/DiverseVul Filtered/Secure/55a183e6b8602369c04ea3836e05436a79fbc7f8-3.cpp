void gf_bifs_check_field_change(GF_Node *node, GF_FieldInfo *field)
{
	if (field->fieldType==GF_SG_VRML_MFNODE) node->sgprivate->flags |= GF_SG_CHILD_DIRTY;
	/*signal node modif*/
	gf_node_changed(node, field);
	/*Notify eventOut in all cases to handle protos*/
	gf_node_event_out(node, field->fieldIndex);
	/*and propagate eventIn if any*/
	if (field->on_event_in) {
		field->on_event_in(node, NULL);
	} else if ((gf_node_get_tag(node) == TAG_MPEG4_Script) && (field->eventType==GF_SG_EVENT_IN)) {
		gf_sg_script_event_in(node, field);
	}

}