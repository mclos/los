##### [中文](https://github.com/mclos/los/blob/master/readme.md)
##### Introduction
1. Los is a c-compiled virtual machine. It uses llvm/clang as its front end, and losld does the backend to compile the source code to generate the los directive file. The Los virtual machine is written in 100% c language and runs on mcu and pc.
Los (core) accounts for about 15Kb of rom.
Los provides a unified API interface to achieve the benefits of writing once, running everywhere, and improving code reusability.

2. [Specific Please See](https://mclos.github.io/)
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

##### Operation Instructions
1. Run a single file, please use the format xxx/los.exe xxx/xx.los
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
