tsize_t t2p_write_pdf_trailer(T2P* t2p, TIFF* output)
{

	tsize_t written = 0;
	char buffer[32];
	int buflen = 0;
	size_t i = 0;

	for (i = 0; i < sizeof(t2p->pdf_fileid) - 8; i += 8)
		snprintf(t2p->pdf_fileid + i, 9, "%.8X", rand());

	written += t2pWriteFile(output, (tdata_t) "trailer\n<<\n/Size ", 17);
	buflen = snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)(t2p->pdf_xrefcount+1));
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n/Root ", 7);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_catalog);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " 0 R \n/Info ", 12);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_info);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) " 0 R \n/ID[<", 11);
	written += t2pWriteFile(output, (tdata_t) t2p->pdf_fileid,
				sizeof(t2p->pdf_fileid) - 1);
	written += t2pWriteFile(output, (tdata_t) "><", 2);
	written += t2pWriteFile(output, (tdata_t) t2p->pdf_fileid,
				sizeof(t2p->pdf_fileid) - 1);
	written += t2pWriteFile(output, (tdata_t) ">]\n>>\nstartxref\n", 16);
	buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_startxref);
	check_snprintf_ret(t2p, buflen, buffer);
	written += t2pWriteFile(output, (tdata_t) buffer, buflen);
	written += t2pWriteFile(output, (tdata_t) "\n%%EOF\n", 7);

	return(written);
}