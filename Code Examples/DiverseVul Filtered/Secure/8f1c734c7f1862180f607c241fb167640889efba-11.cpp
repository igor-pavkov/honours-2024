        Status validateElementInfo(Buffer* buffer,
                                   ValidationState::State* nextState,
                                   BSONElement idElem) {
            Status status = Status::OK();

            signed char type;
            if ( !buffer->readNumber<signed char>(&type) )
                return makeError("invalid bson", idElem);

            if ( type == EOO ) {
                *nextState = ValidationState::EndObj;
                return Status::OK();
            }

            StringData name;
            status = buffer->readCString( &name );
            if ( !status.isOK() )
                return status;

            switch ( type ) {
            case MinKey:
            case MaxKey:
            case jstNULL:
            case Undefined:
                return Status::OK();

            case jstOID:
                if ( !buffer->skip( sizeof(OID) ) )
                    return makeError("invalid bson", idElem);
                return Status::OK();

            case NumberInt:
                if ( !buffer->skip( sizeof(int32_t) ) )
                    return makeError("invalid bson", idElem);
                return Status::OK();

            case Bool:
                if ( !buffer->skip( sizeof(int8_t) ) )
                    return makeError("invalid bson", idElem);
                return Status::OK();


            case NumberDouble:
            case NumberLong:
            case Timestamp:
            case Date:
                if ( !buffer->skip( sizeof(int64_t) ) )
                    return makeError("invalid bson", idElem);
                return Status::OK();

            case DBRef:
                status = buffer->readUTF8String( NULL );
                if ( !status.isOK() )
                    return status;
                buffer->skip( sizeof(OID) );
                return Status::OK();

            case RegEx:
                status = buffer->readCString( NULL );
                if ( !status.isOK() )
                    return status;
                status = buffer->readCString( NULL );
                if ( !status.isOK() )
                    return status;

                return Status::OK();

            case Code:
            case Symbol:
            case String:
                status = buffer->readUTF8String( NULL );
                if ( !status.isOK() )
                    return status;
                return Status::OK();

            case BinData: {
                int sz;
                if ( !buffer->readNumber<int>( &sz ) )
                    return makeError("invalid bson", idElem);
                if ( !buffer->skip( 1 + sz ) )
                    return makeError("invalid bson", idElem);
                return Status::OK();
            }
            case CodeWScope:
                *nextState = ValidationState::BeginCodeWScope;
                return Status::OK();
            case Object:
            case Array:
                *nextState = ValidationState::BeginObj;
                return Status::OK();

            default:
                return makeError("invalid bson type", idElem);
            }
        }