# CMake file for the AHT20 sensor driver

zephyr_library_named(aht20)

zephyr_syscall_include_directories(./)
zephyr_include_directories(./)

zephyr_library_sources_ifdef(CONFIG_AHT20 aht20.c)