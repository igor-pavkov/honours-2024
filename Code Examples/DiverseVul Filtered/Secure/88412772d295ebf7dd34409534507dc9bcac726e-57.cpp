int XMLRPC_VectorRemoveValue(XMLRPC_VALUE vector, XMLRPC_VALUE value) {
   if(vector && vector->v && vector->v->q && value) {
       q_iter qi = Q_Iter_Head_F(vector->v->q);

       while(qi) {
          XMLRPC_VALUE xIter = Q_Iter_Get_F(qi);
          if(xIter == value) {
             XMLRPC_CleanupValue(xIter);
             Q_Iter_Del(vector->v->q, qi);
             return 1;
          }
          qi = Q_Iter_Next_F(qi);
       }
   }
   return 0;
}