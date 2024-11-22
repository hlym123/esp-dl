#include "dl_recognition_database.hpp"
#include "esp_spiffs.h"
//#include "esp_vfs_fat.h"


static const char *TAG = "dl::recognition::DataBase";


#define CONFIG_BSP_SPIFFS_MOUNT_POINT "/spiffs"
#define CONFIG_BSP_SPIFFS_PARTITION_LABEL "storage" 
#define CONFIG_BSP_SPIFFS_MAX_FILES 5

/**************************************************************************************************
 *
 * SPIFFS Function
 *
 **************************************************************************************************/
esp_err_t bsp_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_BSP_SPIFFS_MOUNT_POINT, // "/spiffs"
        .partition_label = CONFIG_BSP_SPIFFS_PARTITION_LABEL, // "storage"
        .max_files = CONFIG_BSP_SPIFFS_MAX_FILES, // 5 
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    //BSP_ERROR_CHECK_RETURN_ERR(ret_val);

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(CONFIG_BSP_SPIFFS_PARTITION_LABEL);
}


namespace dl {
namespace recognition {

	
/*FatFsFlashDataBase::FatFsFlashDataBase(int feat_len, const char *name) : DataBase(name)
{
    this->init(feat_len);
}

FatFsFlashDataBase::~FatFsFlashDataBase()
{
    this->deinit();
}

esp_err_t FatFsFlashDataBase::mount()
{
    snprintf(this->db_path, sizeof(db_path), "%s/%s.db", CONFIG_SPIFLASH_MOUNT_POINT, this->name);
    esp_vfs_fat_mount_config_t mount_config;
    memset(&mount_config, 0, sizeof(esp_vfs_fat_mount_config_t));
    mount_config.max_files = 5;
    mount_config.format_if_mount_failed = true;
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount_rw_wl(
        CONFIG_SPIFLASH_MOUNT_POINT, CONFIG_SPIFLASH_MOUNT_PARTITION, &mount_config, &this->wl_handle));
    return ESP_OK;
}

esp_err_t FatFsFlashDataBase::unmount()
{
    ESP_RETURN_ON_ERROR(
        esp_vfs_fat_spiflash_unmount_rw_wl(CONFIG_SPIFLASH_MOUNT_POINT, this->wl_handle), TAG, "Failed to unmount.");
    return ESP_OK;
}

esp_err_t FatFsFlashDataBase::check_enough_free_space()
{
    uint64_t total_bytes, free_bytes;
    ESP_ERROR_CHECK(esp_vfs_fat_info(CONFIG_SPIFLASH_MOUNT_POINT, &total_bytes, &free_bytes));

    // sector_size <= allocation unit <= sector_size * 128. For wear_levelling, sector size is determined by
    // CONFIG_WL_SECTOR_SIZE option.
    if (free_bytes < CONFIG_WL_SECTOR_SIZE * 128) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

FatFsSDCardDataBase::FatFsSDCardDataBase(int feat_len, const char *name) : DataBase(name)
{
    this->init(feat_len);
}

FatFsSDCardDataBase::~FatFsSDCardDataBase()
{
    this->deinit();
}

esp_err_t FatFsSDCardDataBase::mount()
{
    // snprintf(this->db_path, sizeof(db_path), "%s/%s.db", CONFIG_BSP_SD_MOUNT_POINT, this->name);
    // return bsp_sdcard_mount();

    snprintf(this->db_path, sizeof(db_path), "%s/%s.db", "/sd", this->name);
    return ESP_FAIL;
}

esp_err_t FatFsSDCardDataBase::unmount()
{
    // return bsp_sdcard_unmount();
    return ESP_FAIL;
}

esp_err_t FatFsSDCardDataBase::check_enough_free_space()
{
    uint64_t total_bytes, free_bytes;
    //ESP_ERROR_CHECK(esp_vfs_fat_info(CONFIG_BSP_SD_MOUNT_POINT, &total_bytes, &free_bytes));
    ESP_ERROR_CHECK(esp_vfs_fat_info("/sd", &total_bytes, &free_bytes));

    // sector_size <= allocation unit <= sector_size * 128. For SD cards, sector size is always 512 bytes.
    if (free_bytes < 512 * 128) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
*/


SPIFFSDataBase::SPIFFSDataBase(int feat_len, const char *name) : DataBase(name)
{
    this->init(feat_len);
}

SPIFFSDataBase::~SPIFFSDataBase()
{
    this->deinit();
}

esp_err_t SPIFFSDataBase::mount()
{
    snprintf(this->db_path, sizeof(db_path), "%s/%s.db", CONFIG_BSP_SPIFFS_MOUNT_POINT, this->name);
    return bsp_spiffs_mount();
}

esp_err_t SPIFFSDataBase::unmount()
{
    return bsp_spiffs_unmount();
    return ESP_FAIL;
}

esp_err_t SPIFFSDataBase::check_enough_free_space()
{
    size_t total_bytes, used_bytes;
    ESP_ERROR_CHECK(esp_spiffs_info(CONFIG_BSP_SPIFFS_PARTITION_LABEL, &total_bytes, &used_bytes));
    //ESP_ERROR_CHECK(esp_spiffs_info("face_recofnition", &total_bytes, &used_bytes));

    // SPIFFS is able to reliably utilize only around 75% of assigned partition space.
    if (used_bytes > 0.75 * total_bytes) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

DB::DB(db_type_t db_type, int feat_len, const char *name)
{
    switch (db_type) {
    case DB_FATFS_FLASH:
  //      this->db = new FatFsFlashDataBase(feat_len, name);
  	printf("db_type: fatfs flash\n");
     	break;
    case DB_FATFS_SDCARD:
  	printf("db_type: fatfs sdcard\n");
//	this->db = new FatFsSDCardDataBase(feat_len, name);
        break;
    case DB_SPIFFS:
  	printf("db_type: spiffs\n");
        this->db = new SPIFFSDataBase(feat_len, name);
        break;
    }
}

DB::~DB()
{
    if (this->db) {
        delete this->db;
        this->db = nullptr;
    }
}
} // namespace recognition
} // namespace dl

