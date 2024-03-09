
Weekday readWeekday(char* strBuffer) {
    switch (strBuffer[0]) {
        case 'M':
            return Monday;
        case 'T':
            if(strBuffer[1]=='i'){
                return Tuesday;
            }
            else{
                return Thursday;
            }
        case 'O':
            return Wednesday;
        case 'F':
            return Friday;
        case 'L':
            return Saturday;
        case 'S':
            return Sunday;
    }
}

