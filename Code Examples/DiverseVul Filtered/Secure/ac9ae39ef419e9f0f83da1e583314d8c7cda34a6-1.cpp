static int unzzip_cat (int argc, char ** argv, int extract)
{
    int done = 0;
    int argn;
    ZZIP_DIR* disk;
    zzip_error_t error;
    
    if (argc == 1)
    {
        printf (__FILE__ " version " ZZIP_PACKAGE_NAME " " ZZIP_PACKAGE_VERSION "\n");
        return EXIT_OK; /* better provide an archive argument */
    }
    
    disk = zzip_dir_open (argv[1], &error);
    if (! disk) {
	fprintf(stderr, "%s: %s\n", argv[1], zzip_strerror(error));
	return exitcode(error);
    }

    if (argc == 2)
    {  /* list all */
	ZZIP_DIRENT entry;
	while(zzip_dir_read(disk, &entry))
	{
	    char* name = entry.d_name;
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "wb", 1);
	    if (! out) {
		DBG3("fopen' %s : %s", name, strerror(errno));
	        if (errno != EISDIR) done = EXIT_ERRORS;
	        continue;
	    }
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
	}
    }
    else
    {   /* list only the matching entries - in order of zip directory */
	ZZIP_DIRENT entry;
	while(zzip_dir_read(disk, &entry))
	{
	    char* name = entry.d_name;
	    for (argn=1; argn < argc; argn++)
	    {
		if (! _zzip_fnmatch (argv[argn], name, 
		    _zzip_FNM_NOESCAPE|_zzip_FNM_PATHNAME|_zzip_FNM_PERIOD))
	        {
	            FILE* out = stdout;
	            if (extract) out = create_fopen(name, "wb", 1);
		    if (! out) {
			DBG3("fopen. %s : %s", name, strerror(errno));
		        if (errno != EISDIR) done = EXIT_ERRORS;
		        continue;
		    }
	            unzzip_cat_file (disk, name, out);
	            if (extract) fclose(out);
		    break; /* match loop */
	        }
	    }
	}
    }
    zzip_dir_close(disk);
    return done;
} 