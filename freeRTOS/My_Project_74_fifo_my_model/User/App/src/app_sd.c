#include "bsp.h"
/*
   http://elm-chan.org/fsw/ff/00index_e.html
   https://blog.csdn.net/ba_wang_mao/article/details/109520086
 */
TCHAR *FF_ERRORS[20];
FATFS *fs;
FATFS fs_obj;
FRESULT sd_err;
FILINFO sd_fno;
FIL sd_file;
DIR sd_dir;

TCHAR cwd[256];
UINT mounted = 0;

uint8_t App_Check_Sd_Status(void);
int mount(void);

/**
 * @description: 初始化SD卡
 * @return {*}
 */
int App_Sd_Init(void)
{
    FF_ERRORS[0] = "FR_OK";
    FF_ERRORS[1] = "FR_DISK_ERR";
    FF_ERRORS[2] = "FR_INT_ERR";
    FF_ERRORS[3] = "FR_NOT_READY";
    FF_ERRORS[4] = "FR_NO_FILE";
    FF_ERRORS[5] = "FR_NO_PATH";
    FF_ERRORS[6] = "FR_INVLAID_NAME";
    FF_ERRORS[7] = "FR_DENIED";
    FF_ERRORS[8] = "FR_EXIST";
    FF_ERRORS[9] = "FR_INVALID_OBJECT";
    FF_ERRORS[10] = "FR_WRITE_PROTECTED";
    FF_ERRORS[11] = "FR_INVALID_DRIVE";
    FF_ERRORS[12] = "FR_NOT_ENABLED";
    FF_ERRORS[13] = "FR_NO_FILESYSTEM";
    FF_ERRORS[14] = "FR_MKFS_ABORTED";
    FF_ERRORS[15] = "FR_TIMEOUT";
    FF_ERRORS[16] = "FR_LOCKED";
    FF_ERRORS[17] = "FR_NOT_ENOUGH_CORE";
    FF_ERRORS[18] = "FR_TOO_MANY_OPEN_FILES";
    FF_ERRORS[19] = "FR_INVALID_PARAMETER";

    /* 等待SD卡插入 */
    while (App_Check_Sd_Status() != 1)
    {
    }

    return MAX_OK;    
}

/**
 * @description: 检测SD卡是否插入状态
 * @return {*}
 */
uint8_t App_Check_Sd_Status(void)
{
    mxc_gpio_cfg_t sd_status_cfg_t = {0};
    sd_status_cfg_t.port = MXC_GPIO0;
    sd_status_cfg_t.mask = MXC_GPIO_PIN_12;
    sd_status_cfg_t.func = MXC_GPIO_FUNC_IN;
    sd_status_cfg_t.pad = MXC_GPIO_PAD_NONE;
    sd_status_cfg_t.vssel = MXC_GPIO_VSSEL_VDDIOH;

    MXC_GPIO_Config(&sd_status_cfg_t);

    if (MXC_GPIO_InGet(MXC_GPIO0, MXC_GPIO_PIN_12) == 0)
    {
        return MAX_ERROR;
    }
    return MAX_OK;
}

/**
 * @description: 挂载SD卡
 * @return {*}
 */
int mount(void)
{
    fs = &fs_obj;

    if ((sd_err = f_mount(fs, "", 1)) != FR_OK)
    { // Mount the default drive to fs now
        printf("Error opening SD card: %s\n", FF_ERRORS[sd_err]);
        f_mount(NULL, "", 0);
    }
    else
    {
        printf("SD card mounted.\n");
        mounted = 1;
    }

    f_getcwd(cwd, sizeof(cwd)); // Set the Current working directory

    return sd_err;
}

int umount()
{
    if ((sd_err = f_mount(NULL, "", 0)) != FR_OK) { //Unmount the default drive from its mount point
        printf("Error unmounting volume: %s\n", FF_ERRORS[sd_err]);
    } else {
        printf("SD card unmounted.\n");
        mounted = 0;
    }

    return sd_err;
}

/**
 * @description: 写入SD卡
 * @param {char} *date_num 写入的数据
 * @return {*}
 */
int App_Sd_Write(char *date_num)
{

    UINT wrote = 0;
    uint32_t fil_size;
    /* 挂载文件系统  */
    if (!mounted)
        mount();
    /* 打开文件夹*/
    sd_err = f_opendir(&sd_dir, "/"); /* 如果不带参数，则从当前目录开始 */
    if (sd_err != FR_OK)
    {
        f_mount(NULL, "", 0);
        return sd_err;
    }
    /* 打开test.csv */
    sd_err = f_open(&sd_file, "test.csv", FA_OPEN_ALWAYS | FA_WRITE);
    if (sd_err != FR_OK)
    {
        printf("Error opening file: %s\n", FF_ERRORS[sd_err]);
        f_mount(NULL, "", 0);
        return sd_err;
    }
    else
    {
        fil_size = f_size(&sd_file);
        sd_err = f_lseek(&sd_file, fil_size);
        /* 向test.csv中写入数据 */
        sd_err = f_write(&sd_file, date_num, strlen(date_num), &wrote);
    }
    /* 关闭文件 数据才真正的写入到 SD 卡 */
    if ((sd_err = f_close(&sd_file)) != FR_OK)
    {
        printf("Error closing file: %s\n", FF_ERRORS[sd_err]);
        return sd_err;
    }
    /* 关闭文件夹 */
    f_closedir(&sd_dir);
    /* 卸载文件系统 f_mount可以上电后仅调用一次 */
/*     f_mount(NULL, "", 0);     */
    umount();
    Bsp_Uart_Init();

    return MAX_OK;
}
