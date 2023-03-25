set(CUR_DIR ".")
set(APP_DIR "app")
set(DSP_DIR "fa")
set(MODULES_DIR "modules")
set(SYS_DIR "sys")

# app_main()
set(srcs
"main.c"
)

# app sources Directory
list(APPEND srcs
"${APP_DIR}/app.cpp"
"${APP_DIR}/task_list.cpp"
"${APP_DIR}/task_system.cpp"
"${APP_DIR}/task_cloud.cpp"
"${APP_DIR}/task_console.cpp"
)

# Design pattern sources Directory
list(APPEND srcs
"${DSP_DIR}/fa.c"
)

# modules sources Directory
list(APPEND srcs
"${MODULES_DIR}/cmd_line.c"
)

# sys sources Directory
list(APPEND srcs
"${SYS_DIR}/sys_soft.c"
"${SYS_DIR}/sys_periphs.c"
"${SYS_DIR}/sys_boot.c"
"${SYS_DIR}/mqtt_conn.c"
)

set(incs
"${CUR_DIR}"
"${APP_DIR}"
"${DSP_DIR}"
"${MODULES_DIR}"
"${SYS_DIR}"
)

idf_component_register( 
    SRCS "${srcs}"
    INCLUDE_DIRS "${incs}"
)
