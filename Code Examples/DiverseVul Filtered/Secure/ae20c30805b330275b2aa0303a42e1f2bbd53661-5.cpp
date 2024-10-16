static int error(std::string& errors, const char* msg, const char* x, const char* y, int z)
{
    static const size_t buffer_size = 512;
    char buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    snprintf(buffer, buffer_size, msg, x, y, z) ;
    if ( errno ) {
        perror(buffer) ;
    } else {
        fprintf(stderr, "%s\n", buffer);
    }
    errors += std::string(msg) + '\n';
    return -1;
}