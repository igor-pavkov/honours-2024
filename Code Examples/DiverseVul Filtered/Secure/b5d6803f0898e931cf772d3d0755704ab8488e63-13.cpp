void t2p_pdf_currenttime(T2P* t2p)
{
	struct tm* currenttime;
	time_t timenow;

	if (time(&timenow) == (time_t) -1) {
		TIFFError(TIFF2PDF_MODULE,
			  "Can't get the current time: %s", strerror(errno));
		timenow = (time_t) 0;
	}

	currenttime = localtime(&timenow);
	snprintf(t2p->pdf_datetime, sizeof(t2p->pdf_datetime),
		 "D:%.4d%.2d%.2d%.2d%.2d%.2d",
		 (currenttime->tm_year + 1900) % 65536,
		 (currenttime->tm_mon + 1) % 256,
		 (currenttime->tm_mday) % 256,
		 (currenttime->tm_hour) % 256,
		 (currenttime->tm_min) % 256,
		 (currenttime->tm_sec) % 256);

	return;
}