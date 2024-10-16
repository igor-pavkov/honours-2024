inline std::unique_ptr<std::pair<std::string, std::string>> qs_dict_name2kv(const char * dict_name, char * const * qs_kv, int qs_kv_size, int nth = 0)
{
    int i;
    size_t name_len, skip_to_eq, skip_to_brace_open, skip_to_brace_close;

    name_len = strlen(dict_name);

#ifdef _qsSORTING
// TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
    for(i=0; i<qs_kv_size; i++)
    {
        if ( strncmp(dict_name, qs_kv[i], name_len) == 0 )
        {
            skip_to_eq = strcspn(qs_kv[i], "=");
            if ( qs_kv[i][skip_to_eq] == '=' )
                skip_to_eq++;
            skip_to_brace_open = strcspn(qs_kv[i], "[");
            if ( qs_kv[i][skip_to_brace_open] == '[' )
                skip_to_brace_open++;
            skip_to_brace_close = strcspn(qs_kv[i], "]");

            if ( skip_to_brace_open <= skip_to_brace_close &&
                 skip_to_brace_open > 0 &&
                 skip_to_brace_close > 0 &&
                 nth == 0 )
            {
                auto key = std::string(qs_kv[i] + skip_to_brace_open, skip_to_brace_close - skip_to_brace_open);
                auto value = std::string(qs_kv[i] + skip_to_eq);
                return std::unique_ptr<std::pair<std::string, std::string>>(new std::pair<std::string, std::string>(key, value));
            }
            else
            {
                --nth;
            }
        }
    }
#endif  // _qsSORTING

    return nullptr;
}