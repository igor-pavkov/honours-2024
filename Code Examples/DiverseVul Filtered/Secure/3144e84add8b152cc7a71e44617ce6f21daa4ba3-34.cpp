FILE *pp_input_fopen(const char *filename, enum file_flags mode)
{
    return inc_fopen(filename, NULL, NULL, INC_OPTIONAL, mode);
}