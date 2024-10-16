static u64 box_unused_bytes(GF_Box *b)
{
	u32 i, count;
	u64 size = 0;
	switch (b->type) {
	case GF_QT_BOX_TYPE_WIDE:
	case GF_ISOM_BOX_TYPE_FREE:
	case GF_ISOM_BOX_TYPE_SKIP:
		size += b->size;
		break;
	}
	count = gf_list_count(b->child_boxes);
	for (i=0; i<count; i++) {
		GF_Box *child = gf_list_get(b->child_boxes, i);
		size += box_unused_bytes(child);
	}
	return size;
}