#ifndef __TIMEFN_H__
#define __TIMEFN_H__

#include <stdint.h>

#define TIMEFN_DATETIMESTRINGSIZE 20
#define TIMEFN_ONEDAYINMILLIS (24L*60L*60L*1000L)

/* Changing this struct will have bad effects on datfile.c, please be careful! */
/* SEARCH for timefntime_t * */
typedef struct {
	uint16_t year, month, day, hour, minute, second;
} timefntime_t;

int64_t timefn_getunixmillis(int year, int month, int day, int hour, int minute, int second);
void timefn_formattimefromdatetimestruct_inplace(char * dst, timefntime_t * datetimestruct);
int64_t timefn_getunixmillisfromtimestruct(timefntime_t * timestruct);
void timefn_gettimefromunixtimemillis(timefntime_t * timeoutput, int64_t unixtimemillis);
char * timefn_formattimefromdatetimestruct(timefntime_t * datetimestruct);
int64_t timefn_getcurrentunixtimemillis();
int64_t timefn_getcurrentunixtime();
void timefn_getcurrenttimedatestruct(timefntime_t * timeoutput);
void timefn_formattimefrommillis(char * dst, int64_t unixtimemillis);
int64_t timefn_parsetimetomillis(char * src);

#endif /* __TIMEFN_H__ */
