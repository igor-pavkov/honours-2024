opj_image_t* rawtoimage(const char *filename, opj_cparameters_t *parameters,
                        raw_cparameters_t *raw_cp)
{
    return rawtoimage_common(filename, parameters, raw_cp, OPJ_TRUE);
}