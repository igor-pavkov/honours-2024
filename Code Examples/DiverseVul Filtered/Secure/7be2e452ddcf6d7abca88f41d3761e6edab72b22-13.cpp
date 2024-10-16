void t2p_pdf_tifftime(T2P* t2p, TIFF* input)
{
	char* datetime;

	if (TIFFGetField(input, TIFFTAG_DATETIME, &datetime) != 0
	    && (strlen(datetime) >= 19) ){
		t2p->pdf_datetime[0]='D';
		t2p->pdf_datetime[1]=':';
		t2p->pdf_datetime[2]=datetime[0];
		t2p->pdf_datetime[3]=datetime[1];
		t2p->pdf_datetime[4]=datetime[2];
		t2p->pdf_datetime[5]=datetime[3];
		t2p->pdf_datetime[6]=datetime[5];
		t2p->pdf_datetime[7]=datetime[6];
		t2p->pdf_datetime[8]=datetime[8];
		t2p->pdf_datetime[9]=datetime[9];
		t2p->pdf_datetime[10]=datetime[11];
		t2p->pdf_datetime[11]=datetime[12];
		t2p->pdf_datetime[12]=datetime[14];
		t2p->pdf_datetime[13]=datetime[15];
		t2p->pdf_datetime[14]=datetime[17];
		t2p->pdf_datetime[15]=datetime[18];
		t2p->pdf_datetime[16] = '\0';
	} else {
		t2p_pdf_currenttime(t2p);
	}

	return;
}