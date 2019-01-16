#ifndef __STDBOOL_H

#define __STDBOOL_H

 

#undef bool

#undef true

#undef false

 

#define bool int

#define false 0

#define true (!false)

 

#endif
