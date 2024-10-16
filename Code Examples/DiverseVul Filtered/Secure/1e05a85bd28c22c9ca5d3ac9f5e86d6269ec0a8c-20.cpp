Client::noteAdaptationAnswer(const Adaptation::Answer &answer)
{
    clearAdaptation(adaptedHeadSource); // we do not expect more messages

    switch (answer.kind) {
    case Adaptation::Answer::akForward:
        handleAdaptedHeader(const_cast<HttpMsg*>(answer.message.getRaw()));
        break;

    case Adaptation::Answer::akBlock:
        handleAdaptationBlocked(answer);
        break;

    case Adaptation::Answer::akError:
        handleAdaptationAborted(!answer.final);
        break;
    }
}