##### [English Language](#jump)
 ![los](https://raw.githubusercontent.com/mclos/los/master/favicon.ico)
##### 简介
1. Los是一个c语言编译型的虚拟机。它使用[llvm/clang](https://llvm.org/)作为其前端，losld做后端对源代码进行编译，生成los指令文件。Los虚拟机是100%c语言编写的，其可运行在mcu与pc上。
los(核心)占rom 8Kb左右(mini)。
los提供统一的api接口，实现一次编写，到处运行的好处，提高代码可重复使用率。
Author's mailbox:lgtbp@126.com.
2. [具体请看](https://mclos.github.io/)
##### 文件简介
```
│  favicon.ico
│  license-en.md
│  license.md
│  Makefile       -编译los.exe的makefile      
│  readme.md
│  
├─arch           -los提供统一的api接口的现实
│          
├─bin
│      as.exe        
│      clang.exe        -los编译器
│      los.exe          -los解释器
│      losld.exe        -los链接
│      losshell.exe     -los的shell
│      
├─eclipse-build-los     --编译.los可运行eclipse项目
│          
├─inc                   -los提供统一的api接口的头文件
│      
├─lib                  -clang的库文件
│                  
├─los                  -los运行的配置文件
│      
├─los-mini                    -这文件是最小实现
│      los.c                  -los解释核心
│      los.h                  -los的头文件
│      main.c                 -los测试文件
│      test.c                 -los测试文件
├─los-code         
│      los.c                  -los解释核心
│      los_fat_config.c      -los的fat api
│      los_fat_config.h
│      los_t.h
│      
├─losfat                   -los的fat文件
│                  
├─mf-build-los-app         --编译.los可运行makefile项目
│      
└─test-file                -体验文件
        los.exe
        test_gpu.los
```
### 迷你版本(los-mini文件夹内)
##### 运行说明
1. 将bin文件添加到系统环境,进入los-mini文件夹内
2. 执行clang编译:```clang -c test.c```
    与```losld -o test test.o```,生成test.los文件，将test.los文件转换为test[]数组
3. 在los-mini文件夹下，输入make命令，生成los.exe文件。运行观看结果
##### api说明:
1.mini版本los，只有以下api：具体使用请看los.h文件内注释,以及参考test.c main.c
```
uint32_t los_quit(losc_t *lp);
void los_arg_clear(losc_t *lp);
uint32_t los_push_arg(losc_t *lp, uint32_t arg);
uint32_t los_call_addr(losc_t *lp, uint32_t addr);
void los_set_function(fun_os f);
uint32_t los_app_first(uint8_t *addr);
```
#### 移植说明
1. 需要把los.h的内存管理函数实现
```
#define lpram_malloc malloc
#define lpram_free free
```
####  [运行过程](https://mclos.github.io/los%E8%BF%B7%E4%BD%A0%E7%89%88%E6%95%99%E7%A8%8B/)

### 功能完善版本
##### 运行说明
1. 运行单个文件，请使用 xxx/los.exe xxx/xx.los的格式运行,或把los后缀的文件，拖到los.exe。
2. 运行losfat中的文件可以直接双击los.exe,它会已losfat的文件作为跟目录

##### 编译los.exe
* 现提供1个方式
   1. 使用makefile

##### 编译.los虚拟机可运行文件
   * 现提供2个方式-注意把bin文件夹加入系统变量
   1. 使用eclipse-build-los
   2. 在mf-build-los-app夹中使用使用make命令

#### 移植说明
1. 不需要管理app功能的，请使用los_app_frist函数作为入口。如los_app_frist("c://test/test.los"),运行rom或ram中的los文件，请使用如los_app_frist("c://test/test@123456") 123456是los在rom或ram中的地址

2. 在arch文件中复制一份los_api文件夹，把各文件的api实现。

3. los的库，在lib文件中

##### [捐助支持](https://mclos.github.io/%E6%8D%90%E5%8A%A9%E6%94%AF%E6%8C%81/)
* Author's mailbox:lgtbp@126.com.

<span id="jump">English Language</span>

##### Introduction
1. Los is a c-compiled virtual machine. It uses [llvm/clang](https://llvm.org/) as its front end, and losld does the backend to compile the source code to generate the los directive file. The Los virtual machine is written in 100% c language and runs on mcu and pc.
Los (core) accounts for about rom 8Kb (mini).
Los provides a unified API interface to achieve the benefits of writing once, running everywhere, and improving code reusability.
Author's mailbox: lgtbp@126.com.
2. [Specific Please See](https://mclos.github.io/en/)
##### Introduction to the file
```
│ favicon.ico
│ license-en.md
│ license.md
│ Makefile - compile los.exe makefile
│ readme.md
│
├─arch -los provides a unified api interface for reality
│
├─bin
│ as.exe
│ clang.exe -los compiler
│ los.exe -los interpreter
│ losld.exe -los link
│ losshell.exe -los shell
│
├─eclipse-build-los -- compile .los to run the eclipse project
│
├─inc -los provides a unified api interface header file
│
├─lib -clang library file
│
├─los -los running configuration file
│
├─los-mini - this file is the minimum implementation
│ los.c -los explains the core
│ los.h -los header file
│ main.c -los test file
│ test.c -los test file
├─los-code
│ los.c -los explains the core
│ los_fat_config.c -los fat api
│ los_fat_config.h
│ los_t.h
│
├─losfat-los fat file
│
├─mf-build-los-app -- compile .los to run the makefile project
│
└─test-file - experience file
        Los.exe
        Test_gpu.los
```
### Mini version (in the los-mini folder)
##### Operation Instructions
1. Add the bin file to the system environment and enter the los-mini folder.
2. Execute clang compilation: ```clang -c test.c```
    With ```losld -o test test.o```, generate the test.los file and convert the test.los file to the test[] array.
3. In the los-mini folder, type the make command to generate the los.exe file. Run the viewing result
##### api Description:
1.mini version los, only the following api: please see the comments in the los.h file for specific use, and refer to test.c main.c
```
Uint32_t los_quit(losc_t *lp);
Void los_arg_clear(losc_t *lp);
Uint32_t los_push_arg(losc_t *lp, uint32_t arg);
Uint32_t los_call_addr(losc_t *lp, uint32_t addr);
Void los_set_function(fun_os f);
Uint32_t los_app_first(uint8_t *addr);
```
#### Migration Instructions
1. Need to implement the memory management function of los.h
```
#define lpram_malloc malloc
#define lpram_free free
```
####  [working process](https://mclos.github.io/en/Los%20mini%20tutorial/)

### Functionally perfect version
##### Operation Instructions
1. To run a single file, run it in xxx/los.exe xxx/xx.los format, or drag the s suffixed file to los.exe.
2. Run the file in losfat and you can double-click los.exe directly, it will be the losfat file as the directory

##### Compiling los.exe
* 1 way available now
   Use makefile

##### Compile the .los virtual machine to run the file
   * Now provide 2 ways - pay attention to add bin folder to system variables
   1. Use eclipse-build-los
   2. Use the make command in the mf-build-los-app folder

#### Migration Instructions
1. If you don't need to manage the app function, please use the los_app_frist function as the entry. For example, los_app_frist("c://test/test.los"), run the los file in rom or ram, please use los_app_frist("c://test/test@123456") 123456 is los in rom or ram the address of

2. Copy a los_api folder in the archive file and implement the api for each file.

3. los library, in the lib file

##### [Donation support](https://mclos.github.io/en/Donation%20support/)
* Author's mailbox:lgtbp@126.com.
