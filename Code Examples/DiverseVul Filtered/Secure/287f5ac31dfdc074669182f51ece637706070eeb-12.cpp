static int de_identify_pict(deark *c)
{
	struct detection_info dti;

	do_detect_version(c, &dti, 0);
	if(dti.file_version==2) {
		return 85;
	}
	else if(dti.file_version==1) {
		if(dti.has_fileheader) return 25;
		return 15;
	}
	return 0;
}