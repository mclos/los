#include "los.h"
#include <time.h>
typedef struct tm_los {
	uint8_t sec;  ///< seconds,  range 0 to 59
	uint8_t min;  ///< minutes, range 0 to 59
	uint8_t hour;  ///< hours, range 0 to 23
	uint8_t mday;  ///< day of the month, range 1 to 31
	uint8_t month;  ///< month, range 0 to 11
	uint8_t week;  ///< day of the week, range 0 to 6
	uint16_t yday;  ///< day in the year, range 0 to 365
	uint16_t year;  ///< The number of years since 2000
} tm_t;
// 返回当前时间
void set_time_los(void *los);
//设置时间
void get_time_los(void *los);
unsigned int lp_log_time_out(void);
