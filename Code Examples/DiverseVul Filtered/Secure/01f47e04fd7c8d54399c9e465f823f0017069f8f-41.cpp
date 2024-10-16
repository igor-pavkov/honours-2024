static int ntop_set_logging_level(lua_State* vm) {
    char *lvlStr;

    ntop->getTrace()->traceEvent(TRACE_INFO, "%s() called", __FUNCTION__);

    if(ntop->getPrefs()->hasCmdlTraceLevel()) return(CONST_LUA_OK);
    if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING))  return(CONST_LUA_ERROR);
    lvlStr = (char*)lua_tostring(vm, 1);
    if(!strcmp(lvlStr, "trace")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_TRACE);
    }
    else if(!strcmp(lvlStr, "debug")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_DEBUG);
    }
    else if(!strcmp(lvlStr, "info")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_INFO);
    }
    else if(!strcmp(lvlStr, "normal")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_NORMAL);
    }
    else if(!strcmp(lvlStr, "warning")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_WARNING);
    }
    else if(!strcmp(lvlStr, "error")){
        ntop->getTrace()->set_trace_level(TRACE_LEVEL_ERROR);
    }
    else{
        return(CONST_LUA_ERROR);
    }

    return(CONST_LUA_OK);
}