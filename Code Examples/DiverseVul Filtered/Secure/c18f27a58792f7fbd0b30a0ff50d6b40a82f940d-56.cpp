Bool TY_(WarnMissingSIInEmittedDocType)( TidyDocImpl* doc )
{
    Bool isXhtml = doc->lexer->isvoyager;
    Node* doctype;
    
    /* Do not warn in XHTML mode */
    if ( isXhtml )
        return no;

    /* Do not warn if emitted doctype is proprietary */
    if ( TY_(HTMLVersionNameFromCode)(doc->lexer->versionEmitted, isXhtml ) == NULL )
        return no;

    /* Do not warn if no SI is possible */
    if ( GetSIFromVers(doc->lexer->versionEmitted) == NULL )
        return no;

    if ( (doctype = TY_(FindDocType)( doc )) != NULL
         && TY_(GetAttrByName)(doctype, "SYSTEM") == NULL )
        return yes;

    return no;
}