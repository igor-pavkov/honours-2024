char get_next_file(int imageno, dircnt_t *dirptr, img_fol_t *img_fol,
                   opj_decompress_parameters *parameters)
{
    char image_filename[OPJ_PATH_LEN], infilename[OPJ_PATH_LEN],
         outfilename[OPJ_PATH_LEN], temp_ofname[OPJ_PATH_LEN];
    char *temp_p, temp1[OPJ_PATH_LEN] = "";

    strcpy(image_filename, dirptr->filename[imageno]);
    fprintf(stderr, "File Number %d \"%s\"\n", imageno, image_filename);
    if (strlen(img_fol->imgdirpath) + strlen(path_separator) + strlen(
                image_filename) + 1 > sizeof(infilename)) {
        return 1;
    }
    strcpy(infilename, img_fol->imgdirpath);
    strcat(infilename, path_separator);
    strcat(infilename, image_filename);
    parameters->decod_format = infile_format(infilename);
    if (parameters->decod_format == -1) {
        return 1;
    }
    if (opj_strcpy_s(parameters->infile, sizeof(parameters->infile),
                     infilename) != 0) {
        return 1;
    }

    /*Set output file*/
    strcpy(temp_ofname, strtok(image_filename, "."));
    while ((temp_p = strtok(NULL, ".")) != NULL) {
        strcat(temp_ofname, temp1);
        sprintf(temp1, ".%s", temp_p);
    }
    if (img_fol->set_out_format == 1) {
        if (strlen(img_fol->imgdirpath) + 1 + strlen(temp_ofname) + 1 + strlen(
                    img_fol->out_format) + 1 > sizeof(outfilename)) {
            return 1;
        }
        strcpy(outfilename, img_fol->imgdirpath);
        strcat(outfilename, "/");
        strcat(outfilename, temp_ofname);
        strcat(outfilename, ".");
        strcat(outfilename, img_fol->out_format);
        if (opj_strcpy_s(parameters->outfile, sizeof(parameters->outfile),
                         outfilename) != 0) {
            return 1;
        }
    }
    return 0;
}