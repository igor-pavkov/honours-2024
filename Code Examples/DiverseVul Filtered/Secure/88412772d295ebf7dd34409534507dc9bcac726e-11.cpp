XMLRPC_VALUE XMLRPC_DupValueNew (XMLRPC_VALUE xSource) {
	XMLRPC_VALUE xReturn = NULL;
	if (xSource) {
		xReturn = XMLRPC_CreateValueEmpty ();
		if (xSource->id.len) {
			XMLRPC_SetValueID (xReturn, xSource->id.str, xSource->id.len);
		}

		switch (xSource->type) {
		case xmlrpc_int:
		case xmlrpc_boolean:
			XMLRPC_SetValueInt (xReturn, xSource->i);
			break;
		case xmlrpc_string:
		case xmlrpc_base64:
			XMLRPC_SetValueString (xReturn, xSource->str.str, xSource->str.len);
			break;
		case xmlrpc_datetime:
			XMLRPC_SetValueDateTime (xReturn, xSource->i);
			break;
		case xmlrpc_double:
			XMLRPC_SetValueDouble (xReturn, xSource->d);
			break;
		case xmlrpc_vector:
			{
				q_iter qi = Q_Iter_Head_F (xSource->v->q);
				XMLRPC_SetIsVector (xReturn, xSource->v->type);

				while (qi) {
					XMLRPC_VALUE xIter = Q_Iter_Get_F (qi);
					XMLRPC_AddValueToVector (xReturn, XMLRPC_DupValueNew (xIter));
					qi = Q_Iter_Next_F (qi);
				}
			}
			break;
		default:
			break;
		}
	}
	return xReturn;
}