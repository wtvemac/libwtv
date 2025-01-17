#include <libwtv.h>
// There is no storage system right now so data is stored as pre-allocated data within the executable.
#include "fart-sound.h"
#include "poop-emoji.h"
#include "lc2nup_hsb.h"
#include "chill_jingle.h"

// NOTE: the state of libwtv is experimental. This may break over time until I get libwtv in a more stable state.
// Interacting with the SDK will end up being close to how you'd interact with libdragon (with some N64-specific calls removed and some WebTV-specific calls added)


// Stolen and converted from ata.h in Windows. Will modify later.
// The numbers are for my reference to match up with the MIPS I'm reading from the box side.
typedef struct __attribute__((__packed__)) {
	// 0
	struct {
		uint16_t Reserved1 : 1;
		uint16_t Retired3 : 1;
		uint16_t ResponseIncomplete : 1;
		uint16_t Retired2 : 3;
		uint16_t FixedDevice : 1;
		uint16_t RemovableMedia : 1;
		uint16_t Retired1 : 7;
		uint16_t DeviceType : 1;
	} GeneralConfiguration;
	// 1
	uint16_t NumCylinders;
	// 2
	uint16_t SpecificConfiguration;
	// 3
	uint16_t NumHeads;
	// 4
	uint16_t Retired1[2];
	// 6
	uint16_t NumSectorsPerTrack;
	// 7
	uint16_t VendorUnique1[3];
	// 10
	uint8_t SerialNumber[20];
	// 20
	uint16_t Retired2[2];
	// 22
	uint16_t Obsolete1;
	// 23
	uint8_t FirmwareRevision[8];
	// 27
	uint8_t ModelNumber[40];
	// 47
	uint8_t MaximumBlockTransfer;
	// 47.5
	uint8_t VendorUnique2;
	// 48
	struct {
		uint16_t FeatureSupported : 1;
		uint16_t Reserved : 15;
	} TrustedComputing;
	// 49
	union {
		struct {
			uint32_t CapabilitiesData : 32;
		};
		struct {
			uint8_t CurrentLongPhysicalSectorAlignment : 2;
			uint8_t ReservedByte49 : 6;
			uint8_t DmaSupported : 1;
			uint8_t LbaSupported : 1;
			uint8_t IordyDisable : 1;
			uint8_t IordySupported : 1;
			uint8_t Reserved1 : 1;
			uint8_t StandybyTimerSupport : 1;
			uint8_t Reserved2 : 2;
			uint16_t ReservedWord50;
		} Capabilities;
	};
	// 51
	uint16_t ObsoleteWords51[2];
	// 53
	uint16_t TranslationFieldsValid : 3;
	uint16_t Reserved3 : 5;
	uint16_t FreeFallControlSensitivity : 8;
	// 54
	uint16_t NumberOfCurrentCylinders;
	// 55
	uint16_t NumberOfCurrentHeads;
	// 56
	uint16_t CurrentSectorsPerTrack;
	// 57
	uint32_t CurrentSectorCapacity;
	// 59
	uint8_t CurrentMultiSectorSetting;
	uint8_t MultiSectorSettingValid : 1;
	uint8_t ReservedByte59 : 3;
	uint8_t SanitizeFeatureSupported : 1;
	uint8_t CryptoScrambleExtCommandSupported : 1;
	uint8_t OverwriteExtCommandSupported : 1;
	uint8_t BlockEraseExtCommandSupported : 1;
	uint32_t UserAddressableSectors;
	uint16_t ObsoleteWord62;
	uint16_t MultiWordDMASupport : 8;
	uint16_t MultiWordDMAActive : 8;
	uint16_t AdvancedPIOModes : 8;
	uint16_t ReservedByte64 : 8;
	uint16_t MinimumMWXferCycleTime;
	uint16_t RecommendedMWXferCycleTime;
	uint16_t MinimumPIOCycleTime;
	uint16_t MinimumPIOCycleTimeIORDY;
	struct {
		uint16_t ZonedCapabilities : 2;
		uint16_t NonVolatileWriteCache : 1;
		uint16_t ExtendedUserAddressableSectorsSupported : 1;
		uint16_t DeviceEncryptsAllUserData : 1;
		uint16_t ReadZeroAfterTrimSupported : 1;
		uint16_t Optional28BitCommandsSupported : 1;
		uint16_t IEEE1667 : 1;
		uint16_t DownloadMicrocodeDmaSupported : 1;
		uint16_t SetMaxSetPasswordUnlockDmaSupported : 1;
		uint16_t WriteBufferDmaSupported : 1;
		uint16_t ReadBufferDmaSupported : 1;
		uint16_t DeviceConfigIdentifySetDmaSupported : 1;
		uint16_t LPSAERCSupported : 1;
		uint16_t DeterministicReadAfterTrimSupported : 1;
		uint16_t CFastSpecSupported : 1;
	} AdditionalSupported;
	uint16_t ReservedWords70[5];
	uint16_t QueueDepth : 5;
	uint16_t ReservedWord75 : 11;
	struct {
		uint16_t Reserved0 : 1;
		uint16_t SataGen1 : 1;
		uint16_t SataGen2 : 1;
		uint16_t SataGen3 : 1;
		uint16_t Reserved1 : 4;
		uint16_t NCQ : 1;
		uint16_t HIPM : 1;
		uint16_t PhyEvents : 1;
		uint16_t NcqUnload : 1;
		uint16_t NcqPriority : 1;
		uint16_t HostAutoPS : 1;
		uint16_t DeviceAutoPS : 1;
		uint16_t ReadLogDMA : 1;
		uint16_t Reserved2 : 1;
		uint16_t CurrentSpeed : 3;
		uint16_t NcqStreaming : 1;
		uint16_t NcqQueueMgmt : 1;
		uint16_t NcqReceiveSend : 1;
		uint16_t DEVSLPtoReducedPwrState : 1;
		uint16_t Reserved3 : 8;
	} SerialAtaCapabilities;
	struct {
		uint16_t Reserved0 : 1;
		uint16_t NonZeroOffsets : 1;
		uint16_t DmaSetupAutoActivate : 1;
		uint16_t DIPM : 1;
		uint16_t InOrderData : 1;
		uint16_t HardwareFeatureControl : 1;
		uint16_t SoftwareSettingsPreservation : 1;
		uint16_t NCQAutosense : 1;
		uint16_t DEVSLP : 1;
		uint16_t HybridInformation : 1;
		uint16_t Reserved1 : 6;
	} SerialAtaFeaturesSupported;
	struct {
		uint16_t Reserved0 : 1;
		uint16_t NonZeroOffsets : 1;
		uint16_t DmaSetupAutoActivate : 1;
		uint16_t DIPM : 1;
		uint16_t InOrderData : 1;
		uint16_t HardwareFeatureControl : 1;
		uint16_t SoftwareSettingsPreservation : 1;
		uint16_t DeviceAutoPS : 1;
		uint16_t DEVSLP : 1;
		uint16_t HybridInformation : 1;
		uint16_t Reserved1 : 6;
	} SerialAtaFeaturesEnabled;
	uint16_t MajorRevision;
	uint16_t MinorRevision;
	struct {
		uint16_t SmartCommands : 1;
		uint16_t SecurityMode : 1;
		uint16_t RemovableMediaFeature : 1;
		uint16_t PowerManagement : 1;
		uint16_t Reserved1 : 1;
		uint16_t WriteCache : 1;
		uint16_t LookAhead : 1;
		uint16_t ReleaseInterrupt : 1;
		uint16_t ServiceInterrupt : 1;
		uint16_t DeviceReset : 1;
		uint16_t HostProtectedArea : 1;
		uint16_t Obsolete1 : 1;
		uint16_t WriteBuffer : 1;
		uint16_t ReadBuffer : 1;
		uint16_t Nop : 1;
		uint16_t Obsolete2 : 1;
		uint16_t DownloadMicrocode : 1;
		uint16_t DmaQueued : 1;
		uint16_t Cfa : 1;
		uint16_t AdvancedPm : 1;
		uint16_t Msn : 1;
		uint16_t PowerUpInStandby : 1;
		uint16_t ManualPowerUp : 1;
		uint16_t Reserved2 : 1;
		uint16_t SetMax : 1;
		uint16_t Acoustics : 1;
		uint16_t BigLba : 1;
		uint16_t DeviceConfigOverlay : 1;
		uint16_t FlushCache : 1;
		uint16_t FlushCacheExt : 1;
		uint16_t WordValid83 : 2;
		uint16_t SmartErrorLog : 1;
		uint16_t SmartSelfTest : 1;
		uint16_t MediaSerialNumber : 1;
		uint16_t MediaCardPassThrough : 1;
		uint16_t StreamingFeature : 1;
		uint16_t GpLogging : 1;
		uint16_t WriteFua : 1;
		uint16_t WriteQueuedFua : 1;
		uint16_t WWN64Bit : 1;
		uint16_t URGReadStream : 1;
		uint16_t URGWriteStream : 1;
		uint16_t ReservedForTechReport : 2;
		uint16_t IdleWithUnloadFeature : 1;
		uint16_t WordValid : 2;
	} CommandSetSupport;
	struct {
		uint16_t SmartCommands : 1;
		uint16_t SecurityMode : 1;
		uint16_t RemovableMediaFeature : 1;
		uint16_t PowerManagement : 1;
		uint16_t Reserved1 : 1;
		uint16_t WriteCache : 1;
		uint16_t LookAhead : 1;
		uint16_t ReleaseInterrupt : 1;
		uint16_t ServiceInterrupt : 1;
		uint16_t DeviceReset : 1;
		uint16_t HostProtectedArea : 1;
		uint16_t Obsolete1 : 1;
		uint16_t WriteBuffer : 1;
		uint16_t ReadBuffer : 1;
		uint16_t Nop : 1;
		uint16_t Obsolete2 : 1;
		uint16_t DownloadMicrocode : 1;
		uint16_t DmaQueued : 1;
		uint16_t Cfa : 1;
		uint16_t AdvancedPm : 1;
		uint16_t Msn : 1;
		uint16_t PowerUpInStandby : 1;
		uint16_t ManualPowerUp : 1;
		uint16_t Reserved2 : 1;
		uint16_t SetMax : 1;
		uint16_t Acoustics : 1;
		uint16_t BigLba : 1;
		uint16_t DeviceConfigOverlay : 1;
		uint16_t FlushCache : 1;
		uint16_t FlushCacheExt : 1;
		uint16_t Resrved3 : 1;
		uint16_t Words119_120Valid : 1;
		uint16_t SmartErrorLog : 1;
		uint16_t SmartSelfTest : 1;
		uint16_t MediaSerialNumber : 1;
		uint16_t MediaCardPassThrough : 1;
		uint16_t StreamingFeature : 1;
		uint16_t GpLogging : 1;
		uint16_t WriteFua : 1;
		uint16_t WriteQueuedFua : 1;
		uint16_t WWN64Bit : 1;
		uint16_t URGReadStream : 1;
		uint16_t URGWriteStream : 1;
		uint16_t ReservedForTechReport : 2;
		uint16_t IdleWithUnloadFeature : 1;
		uint16_t Reserved4 : 2;
	} CommandSetActive;
	uint16_t UltraDMASupport : 8;
	uint16_t UltraDMAActive : 8;
	struct {
		uint16_t TimeRequired : 15;
		uint16_t ExtendedTimeReported : 1;
	} NormalSecurityEraseUnit;
	struct {
		uint16_t TimeRequired : 15;
		uint16_t ExtendedTimeReported : 1;
	} EnhancedSecurityEraseUnit;
	uint16_t CurrentAPMLevel : 8;
	uint16_t ReservedWord91 : 8;
	uint16_t MasterPasswordID;
	uint16_t HardwareResetResult;
	uint16_t CurrentAcousticValue : 8;
	uint16_t RecommendedAcousticValue : 8;
	uint16_t StreamMinRequestSize;
	uint16_t StreamingTransferTimeDMA;
	uint16_t StreamingAccessLatencyDMAPIO;
	uint32_t StreamingPerfGranularity;
	uint32_t Max48BitLBA[2];
	uint16_t StreamingTransferTime;
	uint16_t DsmCap;
	struct {
		uint16_t LogicalSectorsPerPhysicalSector : 4;
		uint16_t Reserved0 : 8;
		uint16_t LogicalSectorLongerThan256Words : 1;
		uint16_t MultipleLogicalSectorsPerPhysicalSector : 1;
		uint16_t Reserved1 : 2;
	} PhysicalLogicalSectorSize;
	uint16_t InterSeekDelay;
	uint16_t WorldWideName[4];
	uint16_t ReservedForWorldWideName128[4];
	uint16_t ReservedForTlcTechnicalReport;
	uint16_t WordsPerLogicalSector[2];
	struct {
		uint16_t ReservedForDrqTechnicalReport : 1;
		uint16_t WriteReadVerify : 1;
		uint16_t WriteUncorrectableExt : 1;
		uint16_t ReadWriteLogDmaExt : 1;
		uint16_t DownloadMicrocodeMode3 : 1;
		uint16_t FreefallControl : 1;
		uint16_t SenseDataReporting : 1;
		uint16_t ExtendedPowerConditions : 1;
		uint16_t Reserved0 : 6;
		uint16_t WordValid : 2;
	} CommandSetSupportExt;
	struct {
		uint16_t ReservedForDrqTechnicalReport : 1;
		uint16_t WriteReadVerify : 1;
		uint16_t WriteUncorrectableExt : 1;
		uint16_t ReadWriteLogDmaExt : 1;
		uint16_t DownloadMicrocodeMode3 : 1;
		uint16_t FreefallControl : 1;
		uint16_t SenseDataReporting : 1;
		uint16_t ExtendedPowerConditions : 1;
		uint16_t Reserved0 : 6;
		uint16_t Reserved1 : 2;
	} CommandSetActiveExt;
	uint16_t ReservedForExpandedSupportandActive[6];
	uint16_t MsnSupport : 2;
	uint16_t ReservedWord127 : 14;
	struct {
		uint16_t SecuritySupported : 1;
		uint16_t SecurityEnabled : 1;
		uint16_t SecurityLocked : 1;
		uint16_t SecurityFrozen : 1;
		uint16_t SecurityCountExpired : 1;
		uint16_t EnhancedSecurityEraseSupported : 1;
		uint16_t Reserved0 : 2;
		uint16_t SecurityLevel : 1;
		uint16_t Reserved1 : 7;
	} SecurityStatus;
	uint16_t ReservedWord129[31];
	struct {
		uint16_t MaximumCurrentInMA : 12;
		uint16_t CfaPowerMode1Disabled : 1;
		uint16_t CfaPowerMode1Required : 1;
		uint16_t Reserved0 : 1;
		uint16_t Word160Supported : 1;
	} CfaPowerMode1;
	uint16_t ReservedForCfaWord161[7];
	uint16_t NominalFormFactor : 4;
	uint16_t ReservedWord168 : 12;
	struct {
		uint16_t SupportsTrim : 1;
		uint16_t Reserved0 : 15;
	} DataSetManagementFeature;
	uint16_t AdditionalProductID[4];
	uint16_t ReservedForCfaWord174[2];
	uint16_t CurrentMediaSerialNumber[30];
	struct {
		uint16_t Supported : 1;
		uint16_t Reserved0 : 1;
		uint16_t WriteSameSuported : 1;
		uint16_t ErrorRecoveryControlSupported : 1;
		uint16_t FeatureControlSuported : 1;
		uint16_t DataTablesSuported : 1;
		uint16_t Reserved1 : 6;
		uint16_t VendorSpecific : 4;
	} SCTCommandTransport;
	uint16_t ReservedWord207[2];
	struct {
		uint16_t AlignmentOfLogicalWithinPhysical : 14;
		uint16_t Word209Supported : 1;
		uint16_t Reserved0 : 1;
	} BlockAlignment;
	uint16_t WriteReadVerifySectorCountMode3Only[2];
	uint16_t WriteReadVerifySectorCountMode2Only[2];
	struct {
		uint16_t NVCachePowerModeEnabled : 1;
		uint16_t Reserved0 : 3;
		uint16_t NVCacheFeatureSetEnabled : 1;
		uint16_t Reserved1 : 3;
		uint16_t NVCachePowerModeVersion : 4;
		uint16_t NVCacheFeatureSetVersion : 4;
	} NVCacheCapabilities;
	uint16_t NVCacheSizeLSW;
	uint16_t NVCacheSizeMSW;
	uint16_t NominalMediaRotationRate;
	uint16_t ReservedWord218;
	struct {
		uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
		uint8_t Reserved;
	} NVCacheOptions;
	uint16_t WriteReadVerifySectorCountMode : 8;
	uint16_t ReservedWord220 : 8;
	uint16_t ReservedWord221;
	struct {
		uint16_t MajorVersion : 12;
		uint16_t TransportType : 4;
	} TransportMajorVersion;
	uint16_t TransportMinorVersion;
	uint16_t ReservedWord224[6];
	uint32_t ExtendedNumberOfUserAddressableSectors[2];
	uint16_t MinBlocksPerDownloadMicrocodeMode03;
	uint16_t MaxBlocksPerDownloadMicrocodeMode03;
	uint16_t ReservedWord236[19];
	uint16_t Signature : 8;
	uint16_t CheckSum : 8;
} ata_identify;

// NOTE: Random functions will be integraded.
#define PHI 0x9e3779b9
static uint32_t Q[4096], c = 362436;
void init_rand(uint32_t x)
{
	int i;

	Q[0] = x;
	Q[1] = x + PHI;
	Q[2] = x + PHI + PHI;

	for (i = 3; i < 4096; i++)
	{
		Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
	}
}
uint32_t rand_cmwc()
{
	uint64_t t, a = 18782LL;
	static uint32_t i = 4095;
	uint32_t x, r = 0xfffffffe;

	i = (i + 1) & 4095;
	t = a * Q[i] + c;
	c = (t >> 32);
	x = t + c;

	if (x < c)
	{
		x++;
		c++;
    }

	return (Q[i] = r - x);
}

// Squares on all 4 corners.
void draw_fiducials()
{
	const display_context_t disp = display_get();

	// NOTE: the way colors are defined will change.

	int size = 20;

	// Top left
	graphics_draw_box(disp, 0, 0, size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVLNK_COLOR));
	// Top right
	graphics_draw_box(disp, (disp->width - size), 0, size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVHDR_COLOR));
	// Bottom left
	graphics_draw_box(disp, 0, (disp->height - size), size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVHDR_COLOR));
	// Bottom right
	graphics_draw_box(disp, (disp->width - size), (disp->height - size), size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVLNK_COLOR));
	
	display_show(disp);
}

void draw_poopie(int x, int y)
{
	const display_context_t disp = display_get();

	// NOTE: the way colors are defined will change.

	for(int dry = 0, sy = y; dry < poop_emoji_height; dry++)
	{
		uint16_t changed = 0;

		for(int drx = 0, sx = x; drx < poop_emoji_width; drx++)
		{
			uint16_t pixel_color = poop_emoji[poop_emoji_start + ((dry * poop_emoji_width) + drx)];

			if(pixel_color != 0x2d80)
			{
				graphics_draw_pixel(disp, sx, sy, pixel_color);

				sx++;
				changed++;
			}
		}

		if(changed > 0)
		{
			sy++;
		}
	}

	display_show(disp);
}

#define kMaxFilenameLen		28
#define DIRECTORY_SEPARATOR '/'
typedef struct FSNode	FSNode;
struct FSNode
{
	FSNode *next;               /* pointer to next file's FSNode */
	FSNode *parent;             /* pointer to parent directory */
	FSNode *first;              /* 0 for object, or pointer to first FSNode in dir */
	char *data;                 /* 0 for directory, or pointer to file data */
	uint32_t dataLength;        /* size of object */
	uint32_t nodeChecksum;
	uint32_t dataChecksum;      /* 0 if data = nil */
	char name[kMaxFilenameLen];
};
typedef struct /* only used by downloader & boot code to verify ROMFS */
{
	uint32_t uint32_count; // find good name?
	uint32_t checksum;
	
} ROMFSHeader;

FSNode* walk_romfs(FSNode* node, const char* file_path)
{
	while(node != NULL)
	{
		char* cur_node_char = (char *)node->name;
		char* cur_path_char = (char *)file_path;
		while(*cur_node_char != NULL && *cur_path_char != DIRECTORY_SEPARATOR && *cur_path_char != NULL)
		{
			// This node doesn't match searched file name, bail and go onto the next (if there is a next).
			if(*cur_path_char != *cur_node_char || (cur_node_char - node->name) >= kMaxFilenameLen)
			{
				goto next_node;
			}

			cur_node_char++;
			cur_path_char++;
		}

		if(node->first != 0)
		{
			return walk_romfs(node->first, (cur_path_char + 1)); // + 1 to go beyond the DIRECTORY_SEPARATOR
		}
		else
		{
			return node;
		}

	next_node:
		node = node->next;
	}

	return NULL;
}

void romfs_tests()
{
	uint32_t* romfs_base = get_bootrom_romfs_base();

	ROMFSHeader* romfs_header = (ROMFSHeader*)((uint32_t)romfs_base - sizeof(ROMFSHeader));

	printf("---------------------------\n");
	printf("HEADER CHECKSUM=0x%08x\n", romfs_header->checksum);
	printf("HEADER SIZE=0x%08x\n", romfs_header->uint32_count);
	printf("---------------------------\n");

	uint32_t* checksum_base = (uint32_t*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - (sizeof(uint32_t) * romfs_header->uint32_count));
	uint32_t checksum = 0x00000000;
	for(uint32_t i = 0; i < romfs_header->uint32_count; i++)
	{
		checksum += *checksum_base;
		checksum_base++;
	}
	printf("CALCULATED CHECKSUM=0x%08x\n", checksum);
	printf("---------------------------\n");

	FSNode* root_node = (FSNode*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - sizeof(FSNode));
	
	//FSNode* found_node = walk_romfs(root_node, "ROM/GMPatches/Data");
	FSNode* found_node = walk_romfs(root_node, "ROM/Sounds/Message.mid");
	if(found_node != NULL)
	{
		printf("found_node->name=%s, found_node->data=%p .. 0x%08x\n", found_node->name, found_node->data, found_node->dataLength);
	}
	else
	{
		printf("FILE NOT FOUND!");
	}
	printf("---------------------------\n");
}

void ide_tests()
{
	printf("\x0a\x0d\x0a\x0d");

	printf("IDE Test:\x0a\x0d");

	if(is_diskful_box())
	{
		printf("\tChecking for HD\x0a\x0d");
		if(ide_primary_hd_exists(IDE_DRIVE0, IDE_DEFAULT_TIMEOUT))
		{
			printf("\t\tYes we have an HD\x0a\x0d");

			printf("\t\tAsking the HD to identify itself\x0a\x0d");

			ide_command_block command_block;

			uint32_t data_length = 1 << IDE_SECTOR_BSHIFT;
			ata_identify* data = (ata_identify*)malloc(data_length);

			ide_setup_command(&command_block, 0, 0x00, 0xec, 0, 0x00, (void*)data, data_length);

			if(ide_primary_handle_command(IDE_PROTO_PIO_IN, command_block))
			{
				printf("\t\t\tsect = %d, heads = %d, cyl = %d, sect*heads*cyl = %d\x0a\x0d", data->NumSectorsPerTrack, data->NumHeads, data->NumCylinders, (data->NumSectorsPerTrack*data->NumHeads*data->NumCylinders));
				printf("\t\t\tdefault capacity in sectors = %u\x0a\x0d", (((data->CurrentSectorCapacity & 0xffff) << 0x10) + (data->CurrentSectorCapacity >> 0x10)));
				printf("\t\t\tLBA capacity in sectors = %u\x0a\x0d", (((data->UserAddressableSectors & 0xffff) << 0x10) + (data->UserAddressableSectors >> 0x10)));
				printf("\t\t\tMaxMultipleSectors = %d\x0a\x0d", data->MaximumBlockTransfer);
				printf("\t\t\tCapabilitiesFlags = 0x%04x\x0a\x0d", (data->CapabilitiesData >> 0x10));
				printf("\t\t\tPIODataXferMode = 0x%02x\x0a\x0d", (data->ObsoleteWords51[0]>>0x08));
				printf("\t\t\tMinPIOXferTimeNoIORDY = %d ns\x0a\x0d", data->MinimumPIOCycleTime);
				printf("\t\t\tMinPIOXferTimeIORDY = %d ns\x0a\x0d", data->MinimumPIOCycleTimeIORDY);
				printf("\t\t\tFirmware Revision: %.8s\x0a\x0d", data->FirmwareRevision);
				printf("\t\t\tDevice Name: %.40s\x0a\x0d", data->ModelNumber);
			}
			else
			{
				printf("\t\tHD failed to identify itself!\x0a\x0d");
			}


			free(data);
		}
		else
		{
			printf("\t\tNo we don't have an HD\x0a\x0d");
		}
	}
	else
	{
		printf("\tThis isn't a system with a disk. Skipping...\x0a\x0d");
	}

}

int main()
{
	set_leds(2); // NOTE: this will change

	core_init_temp(); // NOTE: this will change

	if(is_solo_box())
	{
		uint8_t reset_cause = 0x00;
		nvram_secondary_read(0x22, &reset_cause, 1);
		if(reset_cause & 0x0c)
		{
			printf("Unplug and re-plug to end up in the bootrom...\x0a\x0d");
			return 0;
		}
	}

	init_rand(2222);

	set_leds(4); // NOTE: this will change

	printf("=============================================================\x0a\x0d");

	display_init(RESOLUTION_560x420, FMT_YUV16, 1, WSRFC_BLACK_COLOR);

	console_alloc(0, 20, 560, 220, HORIZONTAL_PADDING, VERTICAL_PADDING, TAB_WIDTH, LINE_FEED_HEIGHT, true);

	printf("COOL BEANS v1.1!\x0a\x0d");
	printf("Built using eMac's WebTV SDK...\x0a\x0d");
	printf("\x0a\x0d");
	printf("This is a custom putchar implemetation (out serial and screen)\x0a\x0d");
	printf("Using printf code from https://github.com/eyalroz/printf\x0a\x0d");
	printf("Screen font from libdragon (16-bit)\x0a\x0d");
	printf("\x0a\x0d");
	printf("CHIP ID:      0x%08x\x0a\x0d", get_chip_id());
	printf("SYSCONFIG:    0x%08x\x0a\x0d", get_sysconfig());
	printf("RAM SIZE:     %d MB\x0a\x0d", (get_memory_size() / 1024 / 1024));
	printf("SSID:         %s\x0a\x0d", get_ssid_string());
	printf("HD LOCK 0x36: %s\x0a\x0d", get_atapwd_random_string());

	console_close();

	printf("HD PASSWORD:  %s\x0a\x0d", get_ata_pwd_string(get_ata_userpwd()));

	printf("Text to screen disabled...\x0a\x0d");

	draw_fiducials();

	printf("Audio init.\x0a\x0d");
	
	BAEMixer test = minibae_init();
	BAEBankToken bank = NULL;
	
	uint32_t* romfs_base = get_bootrom_romfs_base();
	FSNode* root_node = (FSNode*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - sizeof(FSNode));
	FSNode* patchbank_node = walk_romfs(root_node, "ROM/GMPatches/Data");
	BAEResult r2 = BAEMixer_AddBankFromMemory(test, patchbank_node->data, patchbank_node->dataLength, &bank);
	printf("r2=0x%08x, bank=%p\x0a\x0d", r2, bank);

	int16_t pVersionMajor = 0;
	int16_t pVersionMinor = 0;
	int16_t pVersionSubMinor = 0;
	BAEResult r3 = BAEMixer_GetBankVersion(test, bank, &pVersionMajor, &pVersionMinor, &pVersionSubMinor);
	printf("r3=0x%08x, pVersionMajor=%08x, pVersionMinor=%08x, pVersionSubMinor=%08x\x0a\x0d", r3, pVersionMajor, pVersionMinor, pVersionSubMinor);

	BAESong song = BAESong_New(test);
	FSNode* midi_node = walk_romfs(root_node, "ROM/Cache/Music/DialingWebTV.mid");
	//BAEResult r4 = BAESong_LoadMidiFromMemory(song, midi_node->data, midi_node->dataLength, true);
	BAEResult r4 = BAESong_LoadMidiFromMemory(song, &chill_jingle_mid, chill_jingle_mid_len, true);
	printf("r4=0x%08x\x0a\x0d", r4);
	BAESong_DisplayInfo(song);

	BAEResult r5 = BAESong_Start(song, 0);
	printf("r5=0x%08x\x0a\x0d", r4);

	//audio_set_outx_buffer_callback(sound_callback);

	romfs_tests();

	ide_tests();

	printf("Enabling keyboard (IR and/or PS2)... Press any key to get its key map.\x0a\x0d");
	controller_init();

	uint32_t chipval = REGISTER_READ(0xa4000004) & 0x3fffffff;

	while(1)
	{
		controller_scan();

		hid_event event_object = dequeue_hid_event(); // NOTE: this currently doesn't debounce.

		if(event_object.source != EVENT_NULL)
		{
			uint16_t keycode = GET_KEYCODE(event_object.data);

			printf("source=%04x, data=%04x, keycode=%04x\x0a\x0d", event_object.source, event_object.data, keycode);

			if(KY_IS_PRESSED(event_object.data))
			{
				switch(keycode)
				{
					case KY_F1:
					case KY_POWER:
						printf("Power cycle box...\x0a\x0d");

						set_leds(7);

						if(is_spot_box())
						{
							printf("Watchdog reset...\x0a\x0d");
							REGISTER_WRITE(0xa4000004, (chipval | 0x00000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0x40000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0x80000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0xc0000000));
							printf("Idling around like an idiot. The watchdog is going to get us soon!\x0a\x0d");
						}
						else
						{
							printf("Setting MEM_CMD to preserve memory and BUS_RESET_CAUSE_SET to reset. This typically only works on a Derby or Solo rev >= 1.3. You may need to unplug the box if this gets stuck...\x0a\x0d");
							REGISTER_WRITE(0xa4005010, 0xd0000000);
							REGISTER_WRITE(0xa40000a8, 0x00000004);
						}

						while(1) { }
						break;

					/*case KY_F2:
						if(is_solo_box())
						{
							printf("Boot to bootrom hit...\x0a\x0d");

							set_leds(7);

							printf("Writing to IIC NVRAM (secondary NVRAM) to indicate a skip to the bootrom.\x0a\x0d");
							set_box_flag(BOX_FLAG_BOOTROM_BOOT, true);
							
							printf("Setting BUS_RESET_CAUSE_SET to reset.\x0a\x0d");
							REGISTER_WRITE(0xa4005010, 0xd0000000);
							REGISTER_WRITE(0xa40000a8, 0x00000004);
							REGISTER_WRITE(0xa0000404, 0x0000000b);
							REGISTER_WRITE(0xa00007c4, 0x00000000);
						}
						else
						{
							printf("Boot to bootrom not available for this box (yet)...\x0a\x0d");
						}
						break;

					case KY_F4:
						printf("Power cycle box via watchdog hit...\x0a\x0d");

						set_leds(7);
						
						printf("Enabling watchdog timer.\x0a\x0d");
						REGISTER_WRITE(0xa4000004, (chipval | 0x00000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0x40000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0x80000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0xc0000000));
						
						printf("Idling around like an idiot. The watchdog is going to get us soon!\x0a\x0d");
						while(1) { }
						break;*/

					case KY_F5:
						printf("Disable screen hit...\x0a\x0d");

						set_leds(7);

						printf("Turning the screen off.\x0a\x0d");
						display_disable();
						break;

					case KY_F6:
						printf("Enable screen hit...\x0a\x0d");

						set_leds(4);

						printf("Turning the screen on.\x0a\x0d");
						display_enable();
						break;

					case KY_F7:
					{
						printf("Very square...\x0a\x0d");

						const display_context_t disp = display_get();
						graphics_draw_box(disp, 100, 100, 100, 100, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WHITE_COLOR));
						display_show(disp);
						break;
					}

					default:
						BAESound sound = BAESound_New(test);
						BAEResult r6 = BAESound_LoadMemorySample(sound, &fart_sound, fart_sound_size, BAE_WAVE_TYPE);
						BAEResult r7 = BAESound_Start(sound, 1, BAE_FIXED_1, 0);
						printf("MattMan sends his condolences...\x0a\x0d");
						draw_poopie((rand_cmwc() % (560 - 48)), (260 + (rand_cmwc() % 112)));
						break;
				}
			}
		}
	}

	return 0;
}