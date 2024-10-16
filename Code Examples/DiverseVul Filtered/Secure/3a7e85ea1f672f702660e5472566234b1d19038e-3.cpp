    Status validateBSON( const char* originalBuffer, uint64_t maxLength ) {
        if ( maxLength < 5 ) {
            return Status( ErrorCodes::InvalidBSON, "bson data has to be at least 5 bytes" );
        }

        Buffer buf( originalBuffer, maxLength );
        return validateBSONIterative( &buf );
    }