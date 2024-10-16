            Status readUTF8String( StringData* out ) {
                int sz;
                if ( !readNumber<int>( &sz ) )
                    return makeError("invalid bson", _idElem);

                if ( out ) {
                    *out = StringData( _buffer + _position, sz );
                }

                if ( !skip( sz - 1 ) )
                    return makeError("invalid bson", _idElem);

                char c;
                if ( !readNumber<char>( &c ) )
                    return makeError("invalid bson", _idElem);

                if ( c != 0 )
                    return makeError("not null terminated string", _idElem);

                return Status::OK();
            }