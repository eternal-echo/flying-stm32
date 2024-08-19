# PIO

## 创建工程

```
pio project init --board genericSTM32F407VET6 --project-option "framework=arduino"
```

## 安装库
```
pio lib install "MPU9250"
```

## 修改
`.pio\libdeps\black_f407ve\STM32duino FreeRTOS\src\FreeRTOSConfig_Default.h`里的`configUSE_CMSIS_RTOS_V2`需要取消注释。尝试了添加头文件的方式，但是不生效。

```c

/* configUSE_CMSIS_RTOS_V2 has to be defined and set to 1 to use CMSIS-RTOSv2 */
#define configUSE_CMSIS_RTOS_V2           1
```