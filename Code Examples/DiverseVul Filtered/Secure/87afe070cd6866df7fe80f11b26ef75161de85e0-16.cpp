static GF_Err do_dump_iod()
{
	GF_Err e = GF_OK;
	GF_InitialObjectDescriptor *iod = (GF_InitialObjectDescriptor *)gf_isom_get_root_od(file);
	if (!iod) {
		M4_LOG(GF_LOG_WARNING, ("File %s has no IOD\n", inName));
	} else {
		char szName[GF_MAX_PATH+10];
		FILE *iodf;
		sprintf(szName, "%s.iod", outfile);
		iodf = gf_fopen(szName, "wb");
		if (!iodf) {
			M4_LOG(GF_LOG_ERROR, ("Cannot open destination %s\n", szName));
			e = GF_IO_ERR;
		} else {
			u8 *desc;
			u32 size;
			GF_BitStream *bs = gf_bs_from_file(iodf, GF_BITSTREAM_WRITE);
			if (gf_odf_desc_write((GF_Descriptor *)iod, &desc, &size)==GF_OK) {
				gf_fwrite(desc, size, iodf);
				gf_free(desc);
			} else {
				M4_LOG(GF_LOG_ERROR, ("Error writing IOD %s\n", szName));
				e = GF_IO_ERR;
			}
			gf_bs_del(bs);
			gf_fclose(iodf);
		}
		gf_odf_desc_del((GF_Descriptor*)iod);
	}
	return e;
}