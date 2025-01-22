#ifndef __LIBWTV_STORAGE_ATA_H
#define __LIBWTV_STORAGE_ATA_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATA_MASTERPWD    "WebTV Networks Inc."
#define ATA_PWD_LEN      32
#define ATA_PWD_RAND_LEN 8

typedef enum
{
	ATA_CMD_NOP = 0x00,
	ATA_CMD_CFA_REQUEST_EXT_ERROR_CODE = 0x03,
	ATA_CMD_DATA_SET_MANAGEMENT = 0x06,
	ATA_CMD_DATA_SET_MANAGEMENT_XL = 0x07,
	ATA_CMD_DEVICE_RESET = 0x08,
	ATA_CMD_REQUEST_SENSE_DATA_EXT = 0x0b,
	ATA_CMD_RECALIBRATE = 0x10, // to 0x1f
	ATA_CMD_GET_PHYSICAL_ELEMENT_STATUS = 0x12,
	ATA_CMD_READ_SECTOR = 0x20, // also 0x21 (but without retry)
	ATA_CMD_READ_LONG = 0x22, // also 0x23 (but without retry)
	ATA_CMD_READ_SECTOR_EXT = 0x24,
	ATA_CMD_READ_DMA_EXT = 0x25,
	ATA_CMD_READ_DMA_QUEUED_EXT = 0x26,
	ATA_CMD_READ_NATIVE_MAX_ADDRESS_EXT = 0x27,
	ATA_CMD_READ_MULTIPLE_EXT = 0x29,
	ATA_CMD_READ_STREAM_DMA_EXT = 0x2a,
	ATA_CMD_READ_STREAM_EXT = 0x2b,
	ATA_CMD_READ_STREAM_EXT = 0x2b,
	ATA_CMD_READ_LOG_EXT = 0x2f,
	ATA_CMD_WRITE_SECTOR = 0x30, // also 0x31 (but without retry)
	ATA_CMD_WRITE_LONG = 0x32, // also 0x33 (but without retry)
	ATA_CMD_WRITE_SECTOR_EXT = 0x34,
	ATA_CMD_WRITE_DMA_EXT = 0x35,
	ATA_CMD_WRITE_DMA_QUEUED_EXT = 0x36,
	ATA_CMD_SET_MAX_ADDRESS_EXT = 0x37,
	ATA_CMD_CFA_WRITE_SECTORS_WITHOUT_ERASE = 0x38,
	ATA_CMD_WRITE_MULTIPLE_EXT = 0x39,
	ATA_CMD_WRITE_STREAM_DMA_EXT = 0x3a,
	ATA_CMD_WRITE_STREAM_EXT = 0x3b,
	ATA_CMD_WRITE_VERIFY = 0x3c,
	ATA_CMD_WRITE_DMA_FUA_EXT = 0x3d,
	ATA_CMD_WRITE_DMA_QUEUED_FUA_EXT = 0x3e,
	ATA_CMD_WRITE_LOG_EXT = 0x3f,
	ATA_CMD_READ_VERIFY = 0x40, // also 0x41 (but without retry)
	ATA_CMD_READ_VERIFY_EXT = 0x42,
	ATA_CMD_ZERO_EXT = 0x44,
	ATA_CMD_WRITE_UNCORRECTABLE_EXT = 0x45,
	ATA_CMD_READ_LOG_DMA_EXT = 0x47,
	ATA_CMD_ZAC_MANAGEMENT_IN = 0x4a,
	ATA_CMD_FORMAT_TRACK = 0x50,
	ATA_CMD_CONFIGURE_STREAM = 0x51,
	ATA_CMD_WRITE_LOG_DMA_EXT = 0x57,
	ATA_CMD_TRUSTED_NONDATA = 0x5b,
	ATA_CMD_TRUSTED_RECEIVE = 0x5c,
	ATA_CMD_TRUSTED_RECEIVE_DMA = 0x5d,
	ATA_CMD_TRUSTED_SEND = 0x5e,
	ATA_CMD_TRUSTED_SEND_DMA = 0x5f,
	ATA_CMD_READ_FPDMA_QUEUED = 0x60,
	ATA_CMD_WRITE_FPDMA_QUEUED = 0x61,
	ATA_CMD_NCQ_NONDATA = 0x63,
	ATA_CMD_SEND_FPDMA_QUEUED = 0x64,
	ATA_CMD_RECEIVE_FPDMA_QUEUED = 0x65,
	ATA_CMD_SEEK = 0x70, // to 0x7f
	ATA_CMD_SET_DATETIME = 0x77,
	ATA_CMD_ACCESSIBLE_MAX_ADDRESS_CONFIG = 0x78,
	ATA_CMD_REMOVE_ELEMENT_TRUNCATE = 0x7c,
	ATA_CMD_REMOVE_ELEMENT_REBUILD = 0x7d,
	ATA_CMD_REMOVE_ELEMENT_MODIFY_ZONES = 0x7e,
	ATA_CMD_CFA_TRANSLATE_SECTOR = 0x87,
	ATA_CMD_EXECUTE_DIAGS = 0x90,
	ATA_CMD_INIT_DEVICE_PARMS = 0x91,
	ATA_CMD_DOWNLOAD_MICROCODE = 0x92,
	ATA_CMD_DOWNLOAD_MICROCODE_DMA = 0x93,
	ATA_ZAC_MANAGEMENT_OUT = 0x9f,
	ATA_CMD_PACKET = 0xa0,
	ATA_CMD_IDENTIFY_PACKET_DEVICE = 0xa1,
	ATA_CMD_SERVICE = 0xa2,
	ATA_CMD_SMART = 0xb0,
	ATA_CMD_DEVICE_CONFIG_OVERLAY = 0xb1,
	ATA_CMD_SET_SECTOR_CONFIG_EXT = 0xb2,
	ATA_CMD_SANITIZE_DEVICE = 0xb4,
	ATA_CMD_NV_CACHE = 0xb6,
	ATA_CMD_CFA_ERASE_SECTORS = 0xc0,
	ATA_CMD_READ_MULTIPLE = 0xc4,
	ATA_CMD_WRITE_MILTIPLE = 0xc5,
	ATA_CMD_SET_MULTIPLE_MODE = 0xc6,
	ATA_CMD_READ_DMA_QUEUED = 0xc7,
	ATA_CMD_READ_DMA = 0xc8, // also 0xc9 (but without retry)
	ATA_CMD_WRITE_DMA = 0xca, // also 0xcb (but without retry)
	ATA_CMD_WRITE_DMA_QUEUED = 0xcc,
	ATA_CMD_CFA_WRITE_MULTIPLE_WITHOUT_ERASE = 0xcd,
	ATA_CMD_WRITE_MULTIPLE_FUA_EXT = 0xce,
	ATA_CMD_CHECK_MEDIA_CARD_TYPE = 0xd1,
	ATA_CMD_GET_MEDIA_STATUS = 0xda,
	ATA_CMD_ACK_MEDIA_CHANGE = 0xdb,
	ATA_CMD_BOOT_POSTBOOT = 0xdc,
	ATA_CMD_BOOT_PREBOOT = 0xdd,
	ATA_CMD_MEDIA_LOCK = 0xde,
	ATA_CMD_MEDIA_UNLOCK = 0xdf,
	ATA_CMD_POW_STANDBY_IMMEDIATE = 0xe0, // also 0x94
	ATA_CMD_POW_IDLE_IMMEDIATE = 0xe1, // also 0x95
	ATA_CMD_POW_STANDBY = 0xe2, // also 0x96
	ATA_CMD_POW_IDLE = 0xe3, // also 0x97
	ATA_CMD_READ_BUFFER = 0xe4,
	ATA_CMD_POW_CHECK_POWER_MODE = 0xe5, // also 0x98
	ATA_CMD_POW_SLEEP = 0xe6, // also 0x99
	ATA_CMD_FLUSH_CACHE = 0xe7,
	ATA_CMD_WRITE_BUFFER = 0xe8,
	ATA_CMD_READ_BUFFER_DMA = 0xe9,
	ATA_CMD_FLUSH_CACHE_EXT = 0xea,
	ATA_CMD_WRITE_BUFFER_DMA = 0xeb,
	ATA_CMD_IDENTIFY_DEVICE = 0xec,
	ATA_CMD_MEDIA_EJECT = 0xed,
	ATA_CMD_IDENTIFY_DEVICE_DMA = 0xee,
	ATA_CMD_SET_FEATURES = 0xef,
	ATA_CMD_SEC_SET_PASSWORD = 0xf1,
	ATA_CMD_SEC_UNLOCK = 0xf2,
	ATA_CMD_SEC_ERASE_PREPARE = 0xf3,
	ATA_CMD_SEC_ERASE_UNIT = 0xf4,
	ATA_CMD_SEC_FREEZE_LOCK = 0xf5,
	ATA_CMD_READ_NATIVE_MAX_ADDRESS = 0xf8,
	ATA_CMD_SET_MAX_ADDRESS = 0xf9,
	ATA_CMD_SEC_DISABLE_PASSWORD = 0xf9,
} ata_command;

typedef struct __attribute__((__packed__))
{
	union
	{
		struct
		{
			uint16_t general_configuration_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t reserved2 : 1;
			uint16_t response_incomplete : 1;
			uint16_t reserved3 : 3;
			uint16_t fixed_device : 1;
			uint16_t removable_media : 1;
			uint16_t reserved4 : 7;
			uint16_t device_type : 1;
		} general_configuration;
	};
	uint16_t num_cylinders;
	uint16_t specific_configuration;
	uint16_t num_heads;
	uint16_t reserved1[2];
	uint16_t num_sectors_per_track;
	uint16_t vendor_unique1[3];
	uint8_t  serial_number[20];
	uint16_t reserved2[3];
	uint8_t  firmware_revision[8];
	uint8_t  model_number[40];
	uint8_t  maximum_block_transfer;
	uint8_t  vendor_unique2;
	union
	{
		struct
		{
			uint16_t trusted_computing_data;
		};
		struct
		{
			uint16_t feature_supported : 1;
			uint16_t reserved1 : 15;
		} trusted_computing;
	};
	union
	{
		struct
		{
			uint32_t capabilities_data;
		};
		struct {
			uint8_t  current_long_physical_sector_alignment : 2;
			uint8_t  reserved1 : 6;
			uint8_t  dma_supported : 1;
			uint8_t  lba_supported : 1;
			uint8_t  iordy_disable : 1;
			uint8_t  iordy_supported : 1;
			uint8_t  reserved2 : 1;
			uint8_t  standyby_timer_support : 1;
			uint8_t  reserved3 : 2;
			uint16_t reserved4;
		} capabilities;
	};
	uint16_t pio_cycle_time_mode;
	uint16_t reserved3;
	uint16_t translation_fields_valid : 3;
	uint16_t reserved4 : 5;
	uint16_t free_fall_control_sensitivity : 8;
	uint16_t number_of_current_cylinders;
	uint16_t number_of_current_heads;
	uint16_t current_sectors_per_track;
	uint32_t current_sector_capacity;
	uint8_t  current_multi_sector_setting;
	uint8_t  multi_sector_setting_valid : 1;
	uint8_t  reserved5 : 3;
	uint8_t  sanitize_feature_supported : 1;
	uint8_t  crypto_scramble_ext_command_supported : 1;
	uint8_t  overwrite_ext_command_supported : 1;
	uint8_t  block_erase_ext_command_supported : 1;
	uint32_t user_addressable_sectors;
	uint16_t reserved6;
	uint16_t multi_word_dma_supported : 8;
	uint16_t multi_word_dma_enabled : 8;
	uint16_t advanced_pio_modes : 8;
	uint16_t reserved7 : 8;
	uint16_t minimum_mwxfer_cycle_time;
	uint16_t recommended_mwxfer_cycle_time;
	uint16_t minimum_pio_cycle_time;
	uint16_t minimum_pio_cycle_time_iordy;
	union
	{
		struct
		{
			uint16_t additional_supported_data;
		};
		struct
		{
			uint16_t zoned_capabilities : 2;
			uint16_t non_volatile_write_cache : 1;
			uint16_t extended_user_addressable_sectors : 1;
			uint16_t device_encrypts_all_user_data : 1;
			uint16_t read_zero_after_trim : 1;
			uint16_t optional28_bit_commands : 1;
			uint16_t ieee1667 : 1;
			uint16_t download_microcode_dma : 1;
			uint16_t set_max_set_password_unlock_dma : 1;
			uint16_t write_buffer_dma : 1;
			uint16_t read_buffer_dma : 1;
			uint16_t device_config_identify_set_dma : 1;
			uint16_t lpsaerc : 1;
			uint16_t deterministic_read_after_trim : 1;
			uint16_t cfast_spec : 1;
		} additional_supported;
	};
	uint16_t reserved8[5];
	uint16_t queue_depth : 5;
	uint16_t reserved9 : 11;
	union
	{
		struct
		{
			uint16_t serial_ata_capabilities_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t sata_gen1 : 1;
			uint16_t sata_gen2 : 1;
			uint16_t sata_gen3 : 1;
			uint16_t reserved2 : 4;
			uint16_t ncq : 1;
			uint16_t hipm : 1;
			uint16_t phy_events : 1;
			uint16_t ncq_unload : 1;
			uint16_t ncq_priority : 1;
			uint16_t host_auto_ps : 1;
			uint16_t device_auto_ps : 1;
			uint16_t read_log_dma : 1;
			uint16_t reserved3 : 1;
			uint16_t current_speed : 3;
			uint16_t ncq_streaming : 1;
			uint16_t ncq_queue_mgmt : 1;
			uint16_t ncq_receive_send : 1;
			uint16_t devsl_pto_reduced_pwr_state : 1;
			uint16_t reserved4 : 8;
		} serial_ata_capabilities;
	};
	union
	{
		struct
		{
			uint16_t serial_ata_features_supported_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t non_zero_offsets : 1;
			uint16_t dma_setup_auto_activate : 1;
			uint16_t dipm : 1;
			uint16_t in_order_data : 1;
			uint16_t hardware_feature_control : 1;
			uint16_t software_settings_preservation : 1;
			uint16_t ncq_autosense : 1;
			uint16_t devslp : 1;
			uint16_t hybrid_information : 1;
			uint16_t reserved2 : 6;
		} serial_ata_features_supported;
	};
	union
	{
		struct
		{
			uint16_t serial_ata_features_enabled_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t non_zero_offsets : 1;
			uint16_t dma_setup_auto_activate : 1;
			uint16_t dipm : 1;
			uint16_t in_order_data : 1;
			uint16_t hardware_feature_control : 1;
			uint16_t software_settings_preservation : 1;
			uint16_t device_auto_ps : 1;
			uint16_t devslp : 1;
			uint16_t hybrid_information : 1;
			uint16_t reserved2 : 6;
		} serial_ata_features_enabled;
	};
	uint16_t major_revision;
	uint16_t minor_revision;
	union
	{
		struct
		{
			uint16_t command_set_supported_data[3];
		};
		struct
		{
			uint16_t smart_commands : 1;
			uint16_t security_mode : 1;
			uint16_t removable_media_feature : 1;
			uint16_t power_management : 1;
			uint16_t reserved1 : 1;
			uint16_t write_cache : 1;
			uint16_t look_ahead : 1;
			uint16_t release_interrupt : 1;
			uint16_t service_interrupt : 1;
			uint16_t device_reset : 1;
			uint16_t host_protected_area : 1;
			uint16_t reserved2 : 1;
			uint16_t write_buffer : 1;
			uint16_t read_buffer : 1;
			uint16_t nop : 1;
			uint16_t reserved3 : 1;
			uint16_t download_microcode : 1;
			uint16_t dma_queued : 1;
			uint16_t cfa : 1;
			uint16_t advanced_pm : 1;
			uint16_t msn : 1;
			uint16_t power_up_in_standby : 1;
			uint16_t manual_power_up : 1;
			uint16_t reserved4 : 1;
			uint16_t set_max : 1;
			uint16_t acoustics : 1;
			uint16_t big_lba : 1;
			uint16_t device_config_overlay : 1;
			uint16_t flush_cache : 1;
			uint16_t flush_cache_ext : 1;
			uint16_t reserved5 : 2;
			uint16_t smart_error_log : 1;
			uint16_t smart_self_test : 1;
			uint16_t media_serial_number : 1;
			uint16_t media_card_pass_through : 1;
			uint16_t streaming_feature : 1;
			uint16_t gp_logging : 1;
			uint16_t write_fua : 1;
			uint16_t write_queued_fua : 1;
			uint16_t wwn64bit : 1;
			uint16_t urg_read_stream : 1;
			uint16_t urg_write_stream : 1;
			uint16_t reserved6 : 2;
			uint16_t idle_with_unload_feature : 1;
			uint16_t reserved7 : 2;
		} command_set_supported;
	};
	union
	{
		struct
		{
			uint16_t command_set_enabled_data[3];
		};
		struct
		{
			uint16_t smart_commands : 1;
			uint16_t security_mode : 1;
			uint16_t removable_media_feature : 1;
			uint16_t power_management : 1;
			uint16_t reserved1 : 1;
			uint16_t write_cache : 1;
			uint16_t look_ahead : 1;
			uint16_t release_interrupt : 1;
			uint16_t service_interrupt : 1;
			uint16_t device_reset : 1;
			uint16_t host_protected_area : 1;
			uint16_t reserved2 : 1;
			uint16_t write_buffer : 1;
			uint16_t read_buffer : 1;
			uint16_t nop : 1;
			uint16_t reserved3 : 1;
			uint16_t download_microcode : 1;
			uint16_t dma_queued : 1;
			uint16_t cfa : 1;
			uint16_t advanced_pm : 1;
			uint16_t msn : 1;
			uint16_t power_up_in_standby : 1;
			uint16_t manual_power_up : 1;
			uint16_t reserved4 : 1;
			uint16_t set_max : 1;
			uint16_t acoustics : 1;
			uint16_t big_lba : 1;
			uint16_t device_config_overlay : 1;
			uint16_t flush_cache : 1;
			uint16_t flush_cache_ext : 1;
			uint16_t reserved5 : 2;
			uint16_t smart_error_log : 1;
			uint16_t smart_self_test : 1;
			uint16_t media_serial_number : 1;
			uint16_t media_card_pass_through : 1;
			uint16_t streaming_feature : 1;
			uint16_t gp_logging : 1;
			uint16_t write_fua : 1;
			uint16_t write_queued_fua : 1;
			uint16_t wwn64bit : 1;
			uint16_t urg_read_stream : 1;
			uint16_t urg_write_stream : 1;
			uint16_t reserved6 : 2;
			uint16_t idle_with_unload_feature : 1;
			uint16_t reserved7 : 2;
		} command_set_enabled;
	};
	uint16_t ultra_dma_supported : 8;
	uint16_t ultra_dma_enabled : 8;
	union
	{
		struct
		{
			uint16_t normal_security_erase_unit_data;
		};
		struct
		{
			uint16_t time_required : 15;
			uint16_t extended_time_reported : 1;
		} normal_security_erase_unit;
	};
	union
	{
		struct
		{
			uint16_t enhanced_security_erase_unit_data;
		};
		struct
		{
			uint16_t time_required : 15;
			uint16_t extended_time_reported : 1;
		} enhanced_security_erase_unit;
	};
	uint16_t current_apm_level : 8;
	uint16_t reserved10 : 8;
	uint16_t master_password_id;
	uint16_t hardware_reset_result;
	uint16_t current_acoustic_value : 8;
	uint16_t recommended_acoustic_value : 8;
	uint16_t stream_min_request_size;
	uint16_t streaming_transfer_time_dma;
	uint16_t streaming_access_latency_dma_pio;
	uint32_t streaming_perf_granularity;
	uint32_t max48_bit_lba[2];
	uint16_t streaming_transfer_time;
	uint16_t dsm_cap;
	union
	{
		struct
		{
			uint16_t physical_logical_sector_size_data;
		};
		struct
		{
			uint16_t logical_sectors_per_physical_sector : 4;
			uint16_t reserved1 : 8;
			uint16_t logical_sector_longer_than256_words : 1;
			uint16_t multiple_logical_sectors_per_physical_sector : 1;
			uint16_t reserved2 : 2;
		} physical_logical_sector_size;
	};
	uint16_t inter_seek_delay;
	uint16_t world_wide_name[4];
	uint16_t reserved11[5];
	uint16_t words_per_logical_sector[2];
	union
	{
		struct
		{
			uint16_t command_set_supported_ext_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t write_read_verify : 1;
			uint16_t write_uncorrectable_ext : 1;
			uint16_t read_write_log_dma_ext : 1;
			uint16_t download_microcode_mode3 : 1;
			uint16_t freefall_control : 1;
			uint16_t sense_data_reporting : 1;
			uint16_t extended_power_conditions : 1;
			uint16_t reserved2 : 8;
		} command_set_supported_ext;
	};
	union
	{
		struct
		{
			uint16_t command_set_enabled_ext_data;
		};
		struct
		{
			uint16_t reserved1 : 1;
			uint16_t write_read_verify : 1;
			uint16_t write_uncorrectable_ext : 1;
			uint16_t read_write_log_dma_ext : 1;
			uint16_t download_microcode_mode3 : 1;
			uint16_t freefall_control : 1;
			uint16_t sense_data_reporting : 1;
			uint16_t extended_power_conditions : 1;
			uint16_t reserved2 : 8;
		} command_set_enabled_ext;
	};
	uint16_t reserved12[6];
	union
	{
		struct
		{
			uint16_t media_status_notification_data;
		};
		struct
		{
			uint16_t supported : 2;
			uint16_t reserved1 : 14;
		} media_status_notification;
	};
	union
	{
		struct
		{
			uint16_t security_status_data;
		};
		struct
		{
			uint16_t supported : 1;
			uint16_t enabled : 1;
			uint16_t locked : 1;
			uint16_t frozen : 1;
			uint16_t count_expired : 1;
			uint16_t enhanced_security_erase_supported : 1;
			uint16_t reserved1 : 2;
			uint16_t level : 1;
			uint16_t reserved2 : 7;
		} security_status;
	};
	uint16_t reserved14[31];
	union
	{
		struct
		{
			uint16_t cfa_power_mode1_data;
		};
		struct
		{
			uint16_t maximum_current_in_ma : 12;
			uint16_t disabled : 1;
			uint16_t required : 1;
			uint16_t reserved1 : 2;
		} cfa_power_mode1;
	};
	uint16_t reserved15[7];
	uint16_t nominal_form_factor : 4;
	uint16_t reserved16 : 12;
	union
	{
		struct
		{
			uint16_t data_set_management_feature_data;
		};
		struct
		{
			uint16_t trim_supported : 1;
			uint16_t reserved1 : 15;
		} data_set_management_feature;
	};
	uint16_t additional_product_id[4];
	uint16_t reserved17[2];
	uint16_t current_media_serial_number[30];
	union
	{
		struct
		{
			uint16_t sct_command_transport_data;
		};
		struct
		{
			uint16_t supported : 1;
			uint16_t reserved1 : 1;
			uint16_t write_same_suported : 1;
			uint16_t error_recovery_control_supported : 1;
			uint16_t feature_control_suported : 1;
			uint16_t data_tables_suported : 1;
			uint16_t reserved2 : 6;
			uint16_t vendor_specific : 4;
		} sct_command_transport;
	};
	uint16_t reserved18[2];
	union
	{
		struct
		{
			uint16_t block_alignment_data;
		};
		struct
		{
			uint16_t alignment_of_logical_within_physical : 14;
			uint16_t reserved1 : 2;
		} block_alignment;
	};
	uint16_t write_read_verify_sector_count_mode3_only[2];
	uint16_t write_read_verify_sector_count_mode2_only[2];
	union
	{
		struct
		{
			uint16_t nv_cache_capabilities_data;
		};
		struct
		{
			uint16_t power_mode_enabled : 1;
			uint16_t reserved1 : 3;
			uint16_t feature_set_enabled : 1;
			uint16_t reserved2 : 3;
			uint16_t power_mode_version : 4;
			uint16_t feature_set_version : 4;
		} nv_cache_capabilities;
	};
	uint16_t nv_cache_size_lsw;
	uint16_t nv_cache_size_msw;
	uint16_t nominal_media_rotation_rate;
	uint16_t reserved19;
	union
	{
		struct
		{
			uint16_t nv_cache_options_data;
		};
		struct
		{
			uint8_t estimated_time_to_spin_up_secs;
			uint8_t reserved1;
		} nv_cache_options;
	};
	uint16_t write_read_verify_sector_count_mode : 8;
	uint16_t reserved20[2];
	union
	{
		struct
		{
			uint16_t transport_major_version_data;
		};
		struct
		{
			uint16_t major_version : 12;
			uint16_t transport_type : 4;
		} transport_major_version;
	};
	uint16_t transport_minor_version;
	uint16_t reserved21[6];
	uint32_t extended_number_of_user_addressable_sectors[2];
	uint16_t min_blocks_per_download_microcode_mode03;
	uint16_t max_blocks_per_download_microcode_mode03;
	uint16_t reserved22[19];
	uint16_t signature : 8;
	uint16_t check_sum : 8;
} ata_identity_t;

typedef struct __attribute__((packed))
{
	uint32_t hi;
	uint32_t lo;
} ata_pwd_random_t;

void ata_init();
void ata_close();
bool ata_enabled();
ata_identity_t ata_get_identity();
bool ata_request_identity(ata_identity_t* identity);
uint8_t* get_ata_userpwd();
char* ata_get_userpwd_string();
ata_pwd_random_t ata_get_userpwd_random();
char* ata_get_userpwd_random_string();
uint8_t* ata_get_masterpwd();

#ifdef __cplusplus
}
#endif

#endif
