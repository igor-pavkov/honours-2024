static int parse_length_modifier(const char *format, size_t len, int *flags_return)
{
    int idx = 0;
    int length_modifier = 0;

    while (idx < len) {
        switch (format[idx]) {
            case 'l':
                BUG_RETURN_VAL(length_modifier & LMOD_SHORT, 0);

                if (length_modifier & LMOD_LONG)
                    length_modifier |= LMOD_LONGLONG;
                else
                    length_modifier |= LMOD_LONG;
                break;
            case 'h':
                BUG_RETURN_VAL(length_modifier & (LMOD_LONG|LMOD_LONGLONG), 0);
                length_modifier |= LMOD_SHORT;
                /* gcc says 'short int' is promoted to 'int' when
                 * passed through '...', so ignored during
                 * processing */
                break;
            case 'z':
                length_modifier |= LMOD_SIZET;
                break;
            default:
                goto out;
        }
        idx++;
    }

out:
    *flags_return = length_modifier;
    return idx;
}