static Bool EndOfInput( TidyDocImpl* doc )
{
    assert( doc->docIn != NULL );
    return ( !doc->docIn->pushed && TY_(IsEOF)(doc->docIn) );
}