static void destroy_parameters(opj_decompress_parameters* parameters)
{
    if (parameters) {
        if (parameters->precision) {
            free(parameters->precision);
            parameters->precision = NULL;
        }

        free(parameters->comps_indices);
        parameters->comps_indices = NULL;
    }
}