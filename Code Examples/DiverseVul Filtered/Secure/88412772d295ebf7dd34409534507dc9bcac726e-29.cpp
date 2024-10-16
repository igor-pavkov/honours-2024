int XMLRPC_SetIsVector(XMLRPC_VALUE value, XMLRPC_VECTOR_TYPE type) {
   int bSuccess = 0;

	if (value) {
		/* we can change the type so long as nothing is currently stored. */
		if(value->type == xmlrpc_vector) {
			if(value->v) {
				if(!Q_Size(value->v->q)) {
					value->v->type = type;
				}
			}
		}
		else {
      value->v = calloc(1, sizeof(STRUCT_XMLRPC_VECTOR));
      if(value->v) {
         value->v->q = (queue*)malloc(sizeof(queue));
         if(value->v->q) {
            Q_Init(value->v->q);
            value->v->type = type;
            value->type = xmlrpc_vector;
            bSuccess = 1;
         }
      }
   }
	}

   return bSuccess;
}