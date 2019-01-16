/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los_fat_config.h"
#define ARRLENs(arr) (sizeof(arr) / sizeof(arr[0]))
#if (defined FAT_CAPI) || (defined WINDOW)
extern uint16_t win_width;
extern uint16_t win_height;
extern uint16_t lcd_width;
extern uint16_t lcd_height;
#include "lcdDriver.h"
/**
 * 
 */
uint8_t *get_ini_data(char *data)
{
    uint8_t len, i;
    len = strlen(data);
    for (i = 0; i < len; i++)
    {
        if (data[i] == ':')
        {
            data[i] = 0;
            return &data[i + 1];
        }
    }
    return 0;
}
extern void lgui_set_lcdw(uint16_t w);
extern void lgui_set_lcdh(uint16_t h);
const char pc_ini_name[][8] = {
    "lcdw",
    "lcdh",
    "com1",
};
/**
 * deal ini data
 */
uint32_t pc_ini_data(char *key, char *value)
{
    uint16_t i;
    int num;
    for (i = 0; i < ARRLENs(pc_ini_name); i++)
    {
        if (0 == strcmp(key, pc_ini_name[i]))
        {
            switch (i)
            {
            case 0:
                win_width = atoi(value);
                lcd_width = (win_width);
#ifdef LOS_LGUI_EN
  lgui.w=lcd_width;
#endif
                win_width += 10;
                break;
            case 1:
                win_height = atoi(value);
                lcd_height = (win_height);
#ifdef LOS_LGUI_EN
  lgui.h=lcd_height;
#endif
                win_height += 90;
                break;
            case 2:
                break;
            }
            return 0;
        }
    }
    return 1;
}
/**
 * 这是在pc上初始化los的外设的
 *   xx/xx/losfile/
 * */
int pc_ini_init(char *path)
{
    uint32_t s = 0, i = 0, len = 0, dlen = 0;
    uint8_t buf[128], line[128], *data, *device;
    sprintf(buf, "%s/los/los.ini", path);
    data = los_fat_read_file(buf, &len);  
    if (data == 0)
        return 1;
    //len++; //添加结束符好
    for (i = 0; i < len; i++)
    {
        if (data[i] == 0 || data[i] == '\n' || data[i] == '\r')
        {
            memset(line, 0, 32);
            memcpy(line, &data[s], i - s); //提取这个
            device = get_ini_data(line);
            pc_ini_data(line, device);
            if (data[i + 1] == '\n')
                i++;
            s = i + 1; //下一个开始
        }
    }
    return 0;
}

#endif