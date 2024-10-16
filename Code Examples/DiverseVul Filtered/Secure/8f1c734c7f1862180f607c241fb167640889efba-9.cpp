            Status readCString( StringData* out ) {
                const void* x = memchr( _buffer + _position, 0, _maxLength - _position );
                if ( !x )
                    return makeError("no end of c-string", _idElem);
                uint64_t len = static_cast<uint64_t>( static_cast<const char*>(x) - ( _buffer + _position ) );

                StringData data( _buffer + _position, len );
                _position += len + 1;

                if ( out ) {
                    *out = data;
                }
                return Status::OK();
            }