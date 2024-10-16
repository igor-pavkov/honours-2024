static void reset_predictor_group(PredictorState *ps, int group_num)
{
    int i;
    for (i = group_num - 1; i < MAX_PREDICTORS; i += 30)
        reset_predict_state(&ps[i]);
}