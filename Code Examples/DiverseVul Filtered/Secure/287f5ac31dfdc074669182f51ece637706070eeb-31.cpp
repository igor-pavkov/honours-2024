static void do_detect_version(deark *c, struct detection_info *dti, int mode)
{
	static const u8 v1pattern[2] = { 0x11, 0x01 };
	static const u8 v2pattern[6] = { 0x00, 0x11, 0x02, 0xff, 0x0c, 0x00 };
	u8 buf[6];
	int v1_nohdr = 0;
	int v2_nohdr = 0;
	int v1_hdr = 0;
	int v2_hdr = 0;

	dti->file_version = 0;
	dti->has_fileheader = 0;

	de_read(buf, 522, sizeof(buf));
	if(!de_memcmp(buf, v2pattern, 6)) {
		v2_hdr = 1;
	}
	else if(!de_memcmp(buf, v1pattern, 2)) {
		v1_hdr = 1;
	}
	else {
		de_read(buf, 10, sizeof(buf));
		if(!de_memcmp(buf, v2pattern, 6)) {
			v2_nohdr = 1;
		}
		else if(!de_memcmp(buf, v1pattern, 2)) {
			v1_nohdr = 1;
		}
	}

	if(!v1_hdr && !v2_hdr && !v1_nohdr && !v2_nohdr) {
		return;
	}

	if(v2_hdr) {
		dti->file_version = 2;
		dti->has_fileheader = 1;
		return;
	}
	else if(v2_nohdr) {
		dti->file_version = 2;
		return;
	}

	if(mode==0) {
		// For v1, check that the file ends as expected
		de_read(buf, c->infile->len-2, 2);
		if(buf[1]==0xff) {
			; // v1 files should end with 0xff
		}
		else if(buf[0]==0xff && buf[1]==0x00) {
			; // But a few have an extra NUL byte at the end
		}
	}

	if(v1_hdr) {
		dti->file_version = 1;
		dti->has_fileheader = 1;
		return;
	}
	else if(v1_nohdr) {
		dti->file_version = 1;
		return;
	}
}