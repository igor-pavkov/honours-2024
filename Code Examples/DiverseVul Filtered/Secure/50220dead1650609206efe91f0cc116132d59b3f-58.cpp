static u8 *fetch_item(__u8 *start, __u8 *end, struct hid_item *item)
{
	u8 b;

	if ((end - start) <= 0)
		return NULL;

	b = *start++;

	item->type = (b >> 2) & 3;
	item->tag  = (b >> 4) & 15;

	if (item->tag == HID_ITEM_TAG_LONG) {

		item->format = HID_ITEM_FORMAT_LONG;

		if ((end - start) < 2)
			return NULL;

		item->size = *start++;
		item->tag  = *start++;

		if ((end - start) < item->size)
			return NULL;

		item->data.longdata = start;
		start += item->size;
		return start;
	}

	item->format = HID_ITEM_FORMAT_SHORT;
	item->size = b & 3;

	switch (item->size) {
	case 0:
		return start;

	case 1:
		if ((end - start) < 1)
			return NULL;
		item->data.u8 = *start++;
		return start;

	case 2:
		if ((end - start) < 2)
			return NULL;
		item->data.u16 = get_unaligned_le16(start);
		start = (__u8 *)((__le16 *)start + 1);
		return start;

	case 3:
		item->size++;
		if ((end - start) < 4)
			return NULL;
		item->data.u32 = get_unaligned_le32(start);
		start = (__u8 *)((__le32 *)start + 1);
		return start;
	}

	return NULL;
}