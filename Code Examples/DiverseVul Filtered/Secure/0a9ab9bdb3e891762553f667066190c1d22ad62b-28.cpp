static void __hidp_copy_session(struct hidp_session *session, struct hidp_conninfo *ci)
{
	memset(ci, 0, sizeof(*ci));
	bacpy(&ci->bdaddr, &session->bdaddr);

	ci->flags = session->flags;
	ci->state = session->state;

	ci->vendor  = 0x0000;
	ci->product = 0x0000;
	ci->version = 0x0000;

	if (session->input) {
		ci->vendor  = session->input->id.vendor;
		ci->product = session->input->id.product;
		ci->version = session->input->id.version;
		if (session->input->name)
			strncpy(ci->name, session->input->name, 128);
		else
			strncpy(ci->name, "HID Boot Device", 128);
	}

	if (session->hid) {
		ci->vendor  = session->hid->vendor;
		ci->product = session->hid->product;
		ci->version = session->hid->version;
		strncpy(ci->name, session->hid->name, 128);
	}
}