int ff_int_from_list_or_default(void *ctx, const char * val_name, int val,
                                const int * array_valid_values, int default_value)
{
    int i = 0, ref_val;

    while (1) {
        ref_val = array_valid_values[i];
        if (ref_val == INT_MAX)
            break;
        if (val == ref_val)
            return val;
        i++;
    }
    /* val is not a valid value */
    av_log(ctx, AV_LOG_DEBUG,
           "%s %d are not supported. Set to default value : %d\n", val_name, val, default_value);
    return default_value;
}