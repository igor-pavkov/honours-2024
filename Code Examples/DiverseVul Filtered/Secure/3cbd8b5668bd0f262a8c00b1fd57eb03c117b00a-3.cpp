static void makebmp(const char *step, const char *tempd, int w, int h, void *data) {
    unsigned int tmp1, tmp2, tmp3, tmp4, y;
    char *fname;
    FILE *f;

    if(!tempd) return;
    if(!(fname = cli_gentemp(tempd))) return;
    if(!(f = fopen(fname, "wb"))) {
	cli_unlink(fname);
	cli_dbgmsg("makebmp: failed to create file %s\n", fname);
	free(fname);
	return;
    }
    cli_writeint32(&tmp1, 0x28 + 0xe + w * h * 4);
    cli_writeint32(&tmp2, (32 << 16) | 1);
    tmp3 = 0;
    cli_writeint32(&tmp4, w * h * 4);
    if(!fwrite("BM", 2, 1, f) ||
       !fwrite(&tmp1, 4, 1, f) ||
       !fwrite("aCaB\x36\x00\x00\x00\x28\x00\x00\x00", 12, 1, f) ||
       !fwrite(&w, 4, 1, f) ||
       !fwrite(&h, 4, 1, f) ||
       !fwrite(&tmp2, 4, 1, f) ||
       !fwrite(&tmp3, 4, 1, f) ||
       !fwrite(&tmp4, 4, 1, f) ||
       !fwrite("\1\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0", 16, 1, f)) {
	fclose(f);
	cli_unlink(fname);
	free(fname);
	cli_dbgmsg("makebmp: failed to write outoput\n");
	return;
    }

    for(y=h-1; y<h; y--)
#if WORDS_BIGENDIAN == 0
	if(!fwrite(&((unsigned int *)data)[y*w], w*4, 1, f))
	    break;
#else
    {
	int x;
	for(x=0; x<w; x++) {
	    cli_writeint32(&tmp1, ((unsigned int *)data)[y*w]);
	    if(!fwrite(&tmp1, 4, 1, f))
		break;
	}
	if(x!=w)
	    break;
    }
#endif
    fclose(f);
    if(y<h)
	cli_unlink(fname);
    else
	cli_dbgmsg("makebmp: Image %s dumped to %s\n", step, fname);
    free(fname);
}