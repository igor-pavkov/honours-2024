static bool isUnsupportedTerm(void) {
    char* term = getenv("TERM");
    if (term == NULL)
        return false;
    for (int j = 0; unsupported_term[j]; ++j)
        if (!strcasecmp(term, unsupported_term[j])) {
            return true;
        }
    return false;
}