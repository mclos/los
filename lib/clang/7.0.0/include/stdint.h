/**
 * @file stdint.h
 * @brief lgui的combobox控件api
 * @details
 * @mainpage
 * @author lp
 * @email lgtbp@126.com
 * @version 1
 * @license Copyright © 2018, lp. All rights reserved.
 *			This project (los) shall not be used for profit by any unit or individual without permission;
 *		if it is used for non-profit use such as personal use and communication,
 *      the author shall be specified and the source shall be marked.
 *         The copyright owner is not liable for any direct, indirect, incidental, special, disciplinary or
 *      consequential damages (including but not limited to the purchase of substitute goods or services;
 *      loss of use, data or profits; or commercial interruption) and then causes any liability theory,
 *      whether Contract, strict liability, or infringement (including negligence or otherwise),
 *      use the Software in any way, even if it is informed of the possibility of such damage.
 */
#ifndef CLIB_API_STDINT_H_
#define CLIB_API_STDINT_H_

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef long int64_t;
typedef unsigned long uint64_t;

typedef uint32_t FILE; ///<文件
typedef uint32_t DIR; ///<文件夾
#define  NULL 0
#define LOS_FILE_FILE 0 ///<文件类型
#define LOS_FILE_DIR 1 ///<文件夾類型
#define LOS_FILE_NAME_LEN 32 ///<文件名字长度

#define INT8_MIN ((int8_t) -128)
#define INT16_MIN ((int16_t) -32768)
#define INT32_MIN ((int32_t) -2147483648)
#define INT64_MIN ((int64_t) -9223372036854775808)

#define INT8_MAX ((int8_t) 127)
#define INT16_MAX ((int16_t) 32767)
#define INT32_MAX ((int32_t) 2147483647)
#define INT64_MAX ((int64_t) 9223372036854775807)

#define UINT8_MAX ((uint8_t) 255)
#define UINT16_MAX ((uint16_t) 65535)
#define UINT32_MAX ((uint32_t) 4294967295)
#define UINT64_MAX ((uint64_t) 18446744073709551615)

/**
 * dir
 */
typedef struct _lf_dirent_t {
	unsigned int d_ino;  ///< inode number 索引节点号
	unsigned short d_off; ///<  offset to this dirent 在目录文件中的偏移
	unsigned short d_reclen; ///<  length of this d_name 文件名长
	unsigned char d_type;  ///<  the type of d_name 文件类型
	char d_name[LOS_FILE_NAME_LEN + 1]; ///<  file name (null-terminated) 文件名，最长255字符
} lf_dirent_t;

/**
 *File seek flags
 */
enum lfs_whence_flags {
	FAT_SEEK_SET = 0,   ///< Seek relative to an absolute position
	FAT_SEEK_CUR = 1,   ///<  Seek relative to the current file position
	FAT_SEEK_END = 2,   ///<  Seek relative to the end of the file
};

#endif
