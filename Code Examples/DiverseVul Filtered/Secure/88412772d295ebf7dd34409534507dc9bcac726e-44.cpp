XMLRPC_Callback XMLRPC_ServerFindMethod(XMLRPC_SERVER server, const char* callName) {
   if(server && callName) {
      q_iter qi = Q_Iter_Head_F(&server->methodlist);
      while( qi ) {
         server_method* sm = Q_Iter_Get_F(qi);
         if(sm && !strcmp(sm->name, callName)) {
            return sm->method;
         }
         qi = Q_Iter_Next_F(qi);
      }
   }
   return NULL;
}