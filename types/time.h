#ifndef TIME_H
#define TIME_H

#include <QDateTime>

#define QueryTimeout       15000  // 15 seconds
#define AckTimeout         16000  // 16 seconds
#define CloseDCTimeout      5000  //  5 seconds
#define ReconnectionTimeout 5000  //  5 seconds

#define ServerTime(messageid)        (messageid >> 32LL)
#define CurrentDeltaTime(servertime) (QDateTime::currentDateTime().toTime_t() - servertime)
#define CurrentTimeStamp              QDateTime::currentDateTime().toTime_t()
#define CurrentDateTime               QDateTime::currentDateTime()

#endif // TIME_H
