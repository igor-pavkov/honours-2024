void *test_a_curve(void *pdata) {
    pthread_pcurve *data = (pthread_pcurve*)pdata;
    /*printf("start pthread %d\n",data->ret);*/
    data->ret = TaggedSpiroCPsToBezier2(data->spiro,0,data->bc);
    /*printf("done\n");*/
    return 0;
}