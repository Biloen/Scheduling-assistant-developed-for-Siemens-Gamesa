Time readTime(char* strBuffer) {
    Time timeConverter;
    char firstHourDigit= strBuffer[0], secondHourDigit=strBuffer[1];
    /*The characters that represent the hours are converted to integers*/
    int a= firstHourDigit-'0', b=secondHourDigit-'0';
    if(a>0){
        a*=10;
    }
    timeConverter.hours=a+b;

    char firstMinuteDigit=strBuffer[3], secondMinuteDigit=strBuffer[4];
    /*The characters that represent the minutes are converted to integers*/
    int c=firstMinuteDigit-'0', d=secondMinuteDigit-'0';
    if(c>0){
        c*=10;
    }
    timeConverter.minutes=c+d;
    return timeConverter;
}
