void (*SSL_get_info_callback(const SSL *ssl)) (const SSL * /* ssl */ ,
                                               int /* type */ ,
                                               int /* val */ ) {
    return ssl->info_callback;
}