##### [English Language](.readme-en.md)
##### 简介
1. Los是一个c语言编译型的虚拟机。它使用llvm/clang作为其前端，losld做后端对源代码进行编译，生成los指令文件。Los虚拟机是100%c语言编写的，其可运行在mcu与pc上。
los(核心)占rom 15Kb左右。
los提供统一的api接口，实现一次编写，到处运行的好处，提高代码可重复使用率。

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

##### 运行说明
1. 运行单个文件，请使用 xxx/los.exe xxx/xx.los的格式运行
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
