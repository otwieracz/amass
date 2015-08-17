STM32F3-DISCOVERY + FreeRTOS Skeleton

Notes:
- Generated using STM32CubeMX
- BSP included
- Stripped out of CubeMX-stuff

* Toolchain:
https://launchpad.net/gcc-arm-embedded

* STM32CubeMX on Linux
(from http://fivevolt.blogspot.com/2014/07/installing-stm32cubemx-on-linux.html)

- Download STM32CubeMX (SetupSTM32CubeMX-4.3.0.exe)
- Unzip .exe file
- Run installer (as root) java -cp . com.izforge.izpack.installer.bootstrap.Installer
- Go to nstall location
- Unzip STM32CubeMX.exe
- Sart STM32CubeMX: java -cp . com.st.microxplorer.maingui.IOConfigurator
- :)

* Generate project using STM32CubeMX

- Generate TrueSTUDIO project
- Generate project again, but using SW4STM32 toolchain
  Why? Because using TrueSTUDIO empty linker script is generated.
- Copy linker script:
  cp "SW4STM32/stm32f3-rtos Configuration/STM32F303VCTx_FLASH.ld" "TrueSTUDIO/stm32f3-rtos Configuration/STM32F303VC_FLASH.ld"
  (or similar)

* Generate Makefile from STM32CubeMX project
(from http://www.ba0sh1.com/stm32cubemx-gcc-makefile/)

- Git is here: https://github.com/baoshi/CubeMX2Makefile
- There is one problem in Makefile, change:
C_DEFS = -D__weak=__attribute__((weak)) -D__packed=__attribute__((__packed__)) -DUSE_HAL_DRIVER -DSTM32F303xC
to
C_DEFS = -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F303xC


