XMLRPC_VALUE XMLRPC_VectorGetValueWithID_Case (XMLRPC_VALUE vector, const char *id,
															  XMLRPC_CASE_COMPARISON id_case) {
   if(vector && vector->v && vector->v->q) {
       q_iter qi = Q_Iter_Head_F(vector->v->q);

       while(qi) {
          XMLRPC_VALUE xIter = Q_Iter_Get_F(qi);
          if(xIter && xIter->id.str) {
             if(id_case == xmlrpc_case_sensitive) {
                if(!strcmp(xIter->id.str, id)) {
                   return xIter;
                }
             }
             else if(id_case == xmlrpc_case_insensitive) {
                if(!strcasecmp(xIter->id.str, id)) {
                   return xIter;
                }
             }
          }
          qi = Q_Iter_Next_F(qi);
       }
   }
   return NULL;
}