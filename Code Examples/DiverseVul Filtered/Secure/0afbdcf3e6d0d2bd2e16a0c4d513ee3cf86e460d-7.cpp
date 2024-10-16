int parse_cmdline_decoder(int argc, char **argv,
                          opj_decompress_parameters *parameters, img_fol_t *img_fol)
{
    /* parse the command line */
    int totlen, c;
    opj_option_t long_option[] = {
        {"ImgDir",    REQ_ARG, NULL, 'y'},
        {"OutFor",    REQ_ARG, NULL, 'O'},
        {"force-rgb", NO_ARG,  NULL, 1},
        {"upsample",  NO_ARG,  NULL, 1},
        {"split-pnm", NO_ARG,  NULL, 1},
        {"threads",   REQ_ARG, NULL, 'T'},
        {"quiet", NO_ARG,  NULL, 1},
    };

    const char optlist[] = "i:o:r:l:x:d:t:p:c:"

                           /* UniPG>> */
#ifdef USE_JPWL
                           "W:"
#endif /* USE_JPWL */
                           /* <<UniPG */
                           "h"     ;

    long_option[2].flag = &(parameters->force_rgb);
    long_option[3].flag = &(parameters->upsample);
    long_option[4].flag = &(parameters->split_pnm);
    long_option[6].flag = &(parameters->quiet);
    totlen = sizeof(long_option);
    opj_reset_options_reading();
    img_fol->set_out_format = 0;
    do {
        c = opj_getopt_long(argc, argv, optlist, long_option, totlen);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 0: /* long opt with flag */
            break;
        case 'i': {         /* input file */
            char *infile = opj_optarg;
            parameters->decod_format = infile_format(infile);
            switch (parameters->decod_format) {
            case J2K_CFMT:
                break;
            case JP2_CFMT:
                break;
            case JPT_CFMT:
                break;
            case -2:
                fprintf(stderr,
                        "!! infile cannot be read: %s !!\n\n",
                        infile);
                return 1;
            default:
                fprintf(stderr,
                        "[ERROR] Unknown input file format: %s \n"
                        "        Known file formats are *.j2k, *.jp2, *.jpc or *.jpt\n",
                        infile);
                return 1;
            }
            if (opj_strcpy_s(parameters->infile, sizeof(parameters->infile), infile) != 0) {
                fprintf(stderr, "[ERROR] Path is too long\n");
                return 1;
            }
        }
        break;

        /* ----------------------------------------------------- */

        case 'o': {         /* output file */
            char *outfile = opj_optarg;
            parameters->cod_format = get_file_format(outfile);
            switch (parameters->cod_format) {
            case PGX_DFMT:
            case PXM_DFMT:
            case BMP_DFMT:
            case TIF_DFMT:
            case RAW_DFMT:
            case RAWL_DFMT:
            case TGA_DFMT:
            case PNG_DFMT:
                break;
            default:
                fprintf(stderr,
                        "Unknown output format image %s [only *.png, *.pnm, *.pgm, *.ppm, *.pgx, *.bmp, *.tif(f), *.raw, *.yuv or *.tga]!!\n",
                        outfile);
                return 1;
            }
            if (opj_strcpy_s(parameters->outfile, sizeof(parameters->outfile),
                             outfile) != 0) {
                fprintf(stderr, "[ERROR] Path is too long\n");
                return 1;
            }
        }
        break;

        /* ----------------------------------------------------- */

        case 'O': {         /* output format */
            char outformat[50];
            char *of = opj_optarg;
            sprintf(outformat, ".%s", of);
            img_fol->set_out_format = 1;
            parameters->cod_format = get_file_format(outformat);
            switch (parameters->cod_format) {
            case PGX_DFMT:
                img_fol->out_format = "pgx";
                break;
            case PXM_DFMT:
                img_fol->out_format = "ppm";
                break;
            case BMP_DFMT:
                img_fol->out_format = "bmp";
                break;
            case TIF_DFMT:
                img_fol->out_format = "tif";
                break;
            case RAW_DFMT:
                img_fol->out_format = "raw";
                break;
            case RAWL_DFMT:
                img_fol->out_format = "rawl";
                break;
            case TGA_DFMT:
                img_fol->out_format = "raw";
                break;
            case PNG_DFMT:
                img_fol->out_format = "png";
                break;
            default:
                fprintf(stderr,
                        "Unknown output format image %s [only *.png, *.pnm, *.pgm, *.ppm, *.pgx, *.bmp, *.tif(f), *.raw, *.yuv or *.tga]!!\n",
                        outformat);
                return 1;
                break;
            }
        }
        break;

        /* ----------------------------------------------------- */


        case 'r': {     /* reduce option */
            sscanf(opj_optarg, "%u", &(parameters->core.cp_reduce));
        }
        break;

        /* ----------------------------------------------------- */


        case 'l': {     /* layering option */
            sscanf(opj_optarg, "%u", &(parameters->core.cp_layer));
        }
        break;

        /* ----------------------------------------------------- */

        case 'h':           /* display an help description */
            decode_help_display();
            return 1;

        /* ----------------------------------------------------- */

        case 'y': {         /* Image Directory path */
            img_fol->imgdirpath = (char*)malloc(strlen(opj_optarg) + 1);
            if (img_fol->imgdirpath == NULL) {
                return 1;
            }
            strcpy(img_fol->imgdirpath, opj_optarg);
            img_fol->set_imgdir = 1;
        }
        break;

        /* ----------------------------------------------------- */

        case 'd': {         /* Input decode ROI */
            size_t size_optarg = (size_t)strlen(opj_optarg) + 1U;
            char *ROI_values = (char*) malloc(size_optarg);
            if (ROI_values == NULL) {
                fprintf(stderr, "[ERROR] Couldn't allocate memory\n");
                return 1;
            }
            ROI_values[0] = '\0';
            memcpy(ROI_values, opj_optarg, size_optarg);
            /*printf("ROI_values = %s [%d / %d]\n", ROI_values, strlen(ROI_values), size_optarg ); */
            parse_DA_values(ROI_values, &parameters->DA_x0, &parameters->DA_y0,
                            &parameters->DA_x1, &parameters->DA_y1);

            free(ROI_values);
        }
        break;

        /* ----------------------------------------------------- */

        case 't': {         /* Input tile index */
            sscanf(opj_optarg, "%u", &parameters->tile_index);
            parameters->nb_tile_to_decode = 1;
        }
        break;

        /* ----------------------------------------------------- */

        case 'x': {         /* Creation of index file */
            if (opj_strcpy_s(parameters->indexfilename, sizeof(parameters->indexfilename),
                             opj_optarg) != 0) {
                fprintf(stderr, "[ERROR] Path is too long\n");
                return 1;
            }
        }
        break;

        /* ----------------------------------------------------- */
        case 'p': { /* Force precision */
            if (!parse_precision(opj_optarg, parameters)) {
                return 1;
            }
        }
        break;

        /* ----------------------------------------------------- */
        case 'c': { /* Componenets */
            const char* iter = opj_optarg;
            while (1) {
                parameters->numcomps ++;
                parameters->comps_indices = (OPJ_UINT32*) realloc(
                                                parameters->comps_indices,
                                                parameters->numcomps * sizeof(OPJ_UINT32));
                parameters->comps_indices[parameters->numcomps - 1] =
                    (OPJ_UINT32) atoi(iter);
                iter = strchr(iter, ',');
                if (iter == NULL) {
                    break;
                }
                iter ++;
            }
        }
        break;
            /* ----------------------------------------------------- */

            /* UniPG>> */
#ifdef USE_JPWL

        case 'W': {         /* activate JPWL correction */
            char *token = NULL;

            token = strtok(opj_optarg, ",");
            while (token != NULL) {

                /* search expected number of components */
                if (*token == 'c') {

                    static int compno;

                    compno = JPWL_EXPECTED_COMPONENTS; /* predefined no. of components */

                    if (sscanf(token, "c=%d", &compno) == 1) {
                        /* Specified */
                        if ((compno < 1) || (compno > 256)) {
                            fprintf(stderr, "ERROR -> invalid number of components c = %d\n", compno);
                            return 1;
                        }
                        parameters->jpwl_exp_comps = compno;

                    } else if (!strcmp(token, "c")) {
                        /* default */
                        parameters->jpwl_exp_comps = compno; /* auto for default size */

                    } else {
                        fprintf(stderr, "ERROR -> invalid components specified = %s\n", token);
                        return 1;
                    };
                }

                /* search maximum number of tiles */
                if (*token == 't') {

                    static int tileno;

                    tileno = JPWL_MAXIMUM_TILES; /* maximum no. of tiles */

                    if (sscanf(token, "t=%d", &tileno) == 1) {
                        /* Specified */
                        if ((tileno < 1) || (tileno > JPWL_MAXIMUM_TILES)) {
                            fprintf(stderr, "ERROR -> invalid number of tiles t = %d\n", tileno);
                            return 1;
                        }
                        parameters->jpwl_max_tiles = tileno;

                    } else if (!strcmp(token, "t")) {
                        /* default */
                        parameters->jpwl_max_tiles = tileno; /* auto for default size */

                    } else {
                        fprintf(stderr, "ERROR -> invalid tiles specified = %s\n", token);
                        return 1;
                    };
                }

                /* next token or bust */
                token = strtok(NULL, ",");
            };
            parameters->jpwl_correct = OPJ_TRUE;
            if (!(parameter->quiet)) {
                fprintf(stdout, "JPWL correction capability activated\n");
                fprintf(stdout, "- expecting %d components\n", parameters->jpwl_exp_comps);
            }
        }
        break;
#endif /* USE_JPWL */
        /* <<UniPG */

        /* ----------------------------------------------------- */
        case 'T': { /* Number of threads */
            if (strcmp(opj_optarg, "ALL_CPUS") == 0) {
                parameters->num_threads = opj_get_num_cpus();
                if (parameters->num_threads == 1) {
                    parameters->num_threads = 0;
                }
            } else {
                sscanf(opj_optarg, "%d", &parameters->num_threads);
            }
        }
        break;

        /* ----------------------------------------------------- */

        default:
            fprintf(stderr, "[WARNING] An invalid option has been ignored.\n");
            break;
        }
    } while (c != -1);

    /* check for possible errors */
    if (img_fol->set_imgdir == 1) {
        if (!(parameters->infile[0] == 0)) {
            fprintf(stderr, "[ERROR] options -ImgDir and -i cannot be used together.\n");
            return 1;
        }
        if (img_fol->set_out_format == 0) {
            fprintf(stderr,
                    "[ERROR] When -ImgDir is used, -OutFor <FORMAT> must be used.\n");
            fprintf(stderr, "Only one format allowed.\n"
                    "Valid format are PGM, PPM, PNM, PGX, BMP, TIF, TIFF, RAW, YUV, and TGA.\n");
            return 1;
        }
        if (!((parameters->outfile[0] == 0))) {
            fprintf(stderr, "[ERROR] options -ImgDir and -o cannot be used together.\n");
            return 1;
        }
    } else {
        if ((parameters->infile[0] == 0) || (parameters->outfile[0] == 0)) {
            fprintf(stderr, "[ERROR] Required parameters are missing\n"
                    "Example: %s -i image.j2k -o image.pgm\n", argv[0]);
            fprintf(stderr, "   Help: %s -h\n", argv[0]);
            return 1;
        }
    }

    return 0;
}