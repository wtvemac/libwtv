#ifndef __LIBWTV_STORAGE_NVRAM_H
#define __LIBWTV_STORAGE_NVRAM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SECONDARY_NVRAM_IIC_ADDRESS 0xa0

#define MCC(c0, c1, c2, c3) \
	(((c0 & 0xff) << 0x18) | ((c1 & 0xff) << 0x10) | ((c2 & 0xff) << 0x08) | ((c3 & 0xff) << 0x00))

typedef enum : uint32_t
{
	NVPRI_NULL = 0,
	NVPRI_TLLY = MCC('t', 'l', 'l', 'y'), // TellyScript                                          from ReadTellyScriptFromNV
	NVPRI_DLSC = MCC('D', 'L', 'S', 'C'), // DialScript
	NVPRI_HN   = MCC(  0,   0, 'h', 'n'), // wtv-head-waiter host                                 from Network::RestoreState
	NVPRI_HP   = MCC(  0,   0, 'h', 'p'), // wtv-head-waiter port                                 from Network::RestoreState
	NVPRI_HF   = MCC(  0,   0, 'h', 'f'), // wtv-head-waiter flags                                from Network::RestoreState
	NVPRI_HL   = MCC(  0,   0, 'h', 'l'), // wtv-head-waiter limit                                from Network::RestoreState
	NVPRI_HNME = MCC('h', 'n', 'm', 'e'), // wtv-head-waiter (old) host
	NVPRI_HPRT = MCC('h', 'p', 'r', 't'), // wtv-head-waiter (old) port
	NVPRI_SN   = MCC(  0,   0, '*', 'n'), // wtv-* host                                           from Network::RestoreState
	NVPRI_SP   = MCC(  0,   0, '*', 'p'), // wtv-* port                                           from Network::RestoreState
	NVPRI_SF   = MCC(  0,   0, '*', 'f'), // wtv-* flags                                          from Network::RestoreState
	NVPRI_SL   = MCC(  0,   0, '*', 'l'), // wtv-* limit                                          from Network::RestoreState
	NVPRI_FLIP = MCC('F', 'L', 'i', 'p'), // wtv-flashrom IP                                      from UpdateFlash
	NVPRI_FLPO = MCC('F', 'L', 'p', 'o'), // wtv-flashrom port                                    from UpdateFlash
	NVPRI_FLTH = MCC('F', 'L', 't', 'h'), // wtv-flashrom path                                    from UpdateFlash
	NVPRI_BOOT = MCC('b', 'o', 'o', 't'), // Boot URL                                             from Network::RestoreBootURL/Network::RestoreState
	NVPRI_PSST = MCC('P', 's', 's', 't'), // Secret tag (old)
	NVPRI_WTCH = MCC('w', 't', 'c', 'h'), // Message Watch URL                                    from Network::RestoreState
	NVPRI_TOUR = MCC('T', 'O', 'U', 'R'), // Tourist Enabled                                      from RestoreTouristState
	NVPRI_DLLI = MCC('D', 'L', 'L', 'I'), // Download Login URL                                   from Network::RestoreState
	NVPRI_DLLS = MCC('D', 'L', 'L', 'S'), // Download List URL                                    from Network::RestoreState
	NVPRI_DIAL = MCC('D', 'i', 'a', 'l'), // Working Dial Number                                  from Network::RestoreState
	NVPRI_FONE = MCC('F', 'O', 'N', 'E'), // Phone Settings                                       from InitPhoneSettings/InstallPhoneSettings/Network::RestoreState
	NVPRI_ANI  = MCC('A', 'N', 'I', ' '), // Telephone number ID (ANI)                            from Network::RestoreState
	NVPRI_SEID = MCC('S', 'E', 'I', 'D'), // Metering Session ID                                  from Network::RestoreMeteringDataFromNVRam
	NVPRI_MCTM = MCC('M', 'C', 'T', 'M'), // Metering Charged Time                                from Network::RestoreMeteringDataFromNVRam
	NVPRI_MFTM = MCC('M', 'F', 'T', 'M'), // Metering Free Time                                   from Network::RestoreMeteringDataFromNVRam
	NVPRI_CLOG = MCC('c', 'l', 'o', 'g'), // Phone Call Log                                       from InitializePhoneLog
	NVPRI_TZNA = MCC('T', 'Z', 'N', 'A'), // Time Zone Name                                       from Clock::RestoreState
	NVPRI_TZOF = MCC('T', 'Z', 'O', 'F'), // Time Zone Offset                                     from Clock::RestoreState
	NVPRI_TZDR = MCC('T', 'Z', 'D', 'R'), // Time Zone DST Rule
	NVPRI_D3EK = MCC('D', '3', 'E', 'K'), // Cookie Encryption Key
	NVPRI_DLOF = MCC('D', 'L', 'O', 'F'), // Data Download Check Time Offset
	NVPRI_MQOF = MCC('M', 'W', 'O', 'F'), // Message Watch Check Time Offset                      from AlarmClock::RestoreState
	NVPRI_DLOC = MCC('D', 'L', 'O', 'C'), // Data Download Enabled
	NVPRI_TVSS = MCC('T', 'V', 'S', 'S'), // TV Signal Source                                     from System::RestoreStatePhase2
	NVPRI_TVAL = MCC('T', 'V', 'A', 'L'), // TV Listings Auto-download (Old)
	NVPRI_UPRQ = MCC('U', 'P', 'R', 'Q'), // Upgrade Path                                         from RestoreUpgradePath
	NVPRI_TVDL = MCC('T', 'V', 'D', 'L'), // TV Listings Auto-download
	NVPRI_TVZP = MCC('T', 'V', 'Z', 'P'), // TV Zip Code                                          from System::RestoreStatePhase1
	NVPRI_TVHM = MCC('T', 'V', 'H', 'M'), // TV Home State                                        from TVHome::RestoreState
	NVPRI_INTR = MCC('I', 'N', 'T', 'R'), // TV Home Intro Version                                from TVHome::RestoreState
	NVPRI_TVCH = MCC('T', 'V', 'C', 'H'), // Current TV Channel                                   from TVController::RestoreState
	NVPRI_TVLH = MCC('T', 'V', 'L', 'H'), // TV List Headend
	NVPRI_TVEC = MCC('T', 'V', 'E', 'C'), // TV Channel Record Flags                              from TVController::RestoreState
	NVPRI_TVBI = MCC('T', 'V', 'B', 'I'), // TV IR Blaster Info                                   from TVController::RestoreState
	NVPRI_CAPW = MCC('C', 'A', 'P', 'W'), // TV Access Control/CAM Password                       from TVAccess::RestoreState
	NVPRI_CAAL = MCC('C', 'A', 'A', 'L'), // TV Access Control/CAM 'Not Rated' Blocked            from TVAccess::RestoreState
	NVPRI_CAPL = MCC('C', 'A', 'P', 'L'), // TV Access Control/CAM Panel Locked                   from TVAccess::RestoreState
	NVPRI_CAVL = MCC('C', 'A', 'V', 'L'), // TV Access Control/CAM PPV Locked                     from TVAccess::RestoreState
	NVPRI_CASL = MCC('C', 'A', 'S', 'L'), // TV Access Control/CAM PPV Spending Limit             from TVAccess::RestoreState
	NVPRI_GALK = MCC('G', 'A', 'L', 'K'), // Game Access Locked                                   from TVAccess::RestoreState
	NVPRI_GARA = MCC('G', 'A', 'R', 'A'), // Game Access ESRB Rating Limit                        from TVAccess::RestoreState
	NVPRI_CAER = MCC('C', 'A', 'E', 'R'), // TV Access/CAM Extended Rating Limits                 from TVAccess::RestoreState
	NVPRI_CATR = MCC('C', 'A', 'T', 'R'), // TV Access/CAM TV Rating Limit                        from TVAccess::RestoreState
	NVPRI_CAMR = MCC('C', 'A', 'M', 'R'), // TV Access/CAM MPAA Rating Limit                      from TVAccess::RestoreState
	NVPRI_DVRP = MCC('D', 'V', 'R', 'P'), // ????                                                 from DVRInterface::RestoreState
	NVPRI_DVRK = MCC('D', 'V', 'R', 'K'), // DVRFsd MPEG ID                                       from DVRInterface::RestoreState
	NVPRI_DIGA = MCC('D', 'i', 'g', 'A'), // Digital Audio Mode                                   from DigitalAudioRestoreState
	NVPRI_TVC2 = MCC('T', 'V', 'C', '2'), // TV Channel Flags                                     from TVController::RestoreState
	NVPRI_TVCM = MCC('T', 'V', 'C', 'M'), // TV Channel Mode                                      from TVController::RestoreState
	NVPRI_RVNT = MCC('R', 'C', 'N', 'T'), // Recent TV Channels                                   from TVRecent::RestoreState
	NVPRI_TVLD = MCC('T', 'V', 'L', 'D'), // TV Log Disabled Mask                                 from TVLog::Open
	NVPRI_VRSI = MCC('V', 'R', 'S', 'I'), // VCR Controller IR Setup Info                         from VCRController::RestoreState
	NVPRI_VRON = MCC('V', 'R', 'O', 'N'), // VCR Controller IR Enabled                            from VCRController::RestoreState
	NVPRI_MLOC = MCC('M', 'L', 'O', 'C'), // Movie Search URL                                     from TVDatabase::RestoreState
	NVPRI_IRBD = MCC('I', 'R', 'B', 'D'), // IR Blaster Delay                                     from IRBlasterDB::RestoreDelay
	NVPRI_IRCS = MCC('I', 'R', 'C', 'S'), // IR Blaster Code Set History                          from IRBlasterDB::RestoreCodeSetHistory
	NVPRI_PWRF = MCC('P', 'W', 'R', 'F'), // Power on Flags                                       from System::PowerOnLoop/System::RestoreStatePhase1/System::RestoreStatePhase2
	NVPRI_COPW = MCC('C', 'O', 'P', 'W'), // Connect On Power Enabled                             from System::RestoreStatePhase1
	NVPRI_COPU = MCC('C', 'O', 'P', 'U'), // Connect On Power URL                                 from System::RestoreStatePhase1
	NVPRI_CORF = MCC('C', 'O', 'R', 'F'), // ????
	NVPRI_PRST = MCC('P', 'r', 'S', 't'), // ????                                                 from System::RestoreStatePhase2
	NVPRI_TLCK = MCC('T', 'L', 'C', 'K'), // TV Access/CAM Blocked Channels Temoraily Unlocked??? from TVAccess::RestoreState
	NVPRI_LOCK = MCC('L', 'O', 'C', 'K'), // TV Access/CAM Blocked Channels Enabled               from TVAccess::RestoreState
	NVPRI_BRDR = MCC('B', 'R', 'D', 'R'), // Screen Border Color                                  from System::RestoreStatePhase2
	NVPRI_LOCP = MCC('l', 'o', 'c', 'p'), // Local Dialup POP Count                               from System::RestoreStatePhase2
	NVPRI_ITVR = MCC('I', 'T', 'V', 'E'), // iTV Enabled                                          from System::RestoreStatePhase2
	NVPRI_SPOS = MCC('S', 'P', 'o', 's'), // Screen Position                                      from VideoDriver::RestoreDisplayState
	NVPRI_CHAN = MCC('C', 'H', 'A', 'N'), // Stored TV Tuner Band
	NVPRI_LANG = MCC('L', 'A', 'N', 'G'), // ????                                                 from UltimateTV
	NVPRI_DVRM = MCC('D', 'V', 'R', 'M'), // ????                                                 from UltimateTV
	NVPRI_FMOD = MCC('F', 'M', 'o', 'd'), // ????                                                 from UltimateTV
	NVPRI_CAMP = MCC('C', 'A', 'M', 'P'), // ????                                                 from UltimateTV
	NVPRI_CATV = MCC('C', 'A', 'T', 'V'), // ????                                                 from UltimateTV
	NVPRI_CAEX = MCC('C', 'A', 'E', 'X'), // ????                                                 from UltimateTV
	NVPRI_CANR = MCC('C', 'A', 'N', 'R'), // ????                                                 from UltimateTV
	NVPRI_WEBL = MCC('W', 'E', 'B', 'L'), // ????                                                 from UltimateTV
	NVPRI_RDVR = MCC('R', 'D', 'V', 'R'), // ????                                                 from UltimateTV
	NVPRI_RDVL = MCC('R', 'D', 'V', 'L'), // ????                                                 from UltimateTV
	NVPRI_WGPS = MCC('W', 'G', 'P', 'S'), // ????                                                 from UltimateTV
	NVPRI_AUDL = MCC('A', 'U', 'D', 'L'), // ????                                                 from UltimateTV
	NVPRI_AUDD = MCC('A', 'U', 'D', 'D'), // ????                                                 from UltimateTV
	NVPRI_ANAM = MCC('A', 'n', 'a', 'm'), // ????                                                 from UltimateTV
	NVPRI_JOYY = MCC('J', 'O', 'Y', 'Y'), // ????                                                 from UltimateTV
	NVPRI_DKON = MCC('D', 'K', 'O', 'N'), // ????                                                 from UltimateTV
	NVPRI_REGD = MCC('R', 'E', 'G', 'D'), // ????                                                 from UltimateTV
	NVPRI_CHTV = MCC('C', 'H', 'T', 'V'), // ????                                                 from UltimateTV
	NVPRI_UPIN = MCC('U', 'P', 'I', 'N'), // ????                                                 from UltimateTV
	NVPRI_UPVR = MCC('U', 'P', 'V', 'R'), // ????                                                 from UltimateTV
	NVPRI_HSCI = MCC('H', 'S', 'C', 'I'), // High Speed????                                       from UltimateTV
	NVPRI_BILD = MCC('B', 'I', 'L', 'D'), // ????                                                 from UltimateTV
	NVPRI_TVUC = MCC('T', 'V', 'U', 'C'),
	NVPRI_TVVI = MCC('T', 'V', 'V', 'I'),
	NVPRI_TVEX = MCC('T', 'V', 'E', 'X'),
	NVPRI_TVID = MCC('T', 'V', 'I', 'D'),
	NVPRI_SPOT = MCC('S', 'P', 'O', 'T'),
	NVPRI_RTCT = MCC('R', 'T', 'C', 'T'),
	NVPRI_TVSC = MCC('T', 'V', 'S', 'C'),
	NVPRI_KARA = MCC('K', 'A', 'R', 'A'),
	NVPRI_VRCM = MCC('V', 'R', 'C', 'M'),
	NVPRI_TVOT = MCC('T', 'V', 'O', 'T'),
	NVPRI_TVMP = MCC('T', 'V', 'M', 'P'),
	NVPRI_VAPT = MCC('V', 'A', 'P', 't'), // Video Ad State                                        from VideoAdd::SaveState/VideoAdd::RestoreState
	NVPRI_EMAC = MCC('E', 'M', 'A', 'C')  // Test setting.
} pri_nvram_setting_key;

typedef enum : uint32_t
{
	NVSEC_FLAGS_HI     = 0x2d,
	NVSEC_FLAGS_LO     = 0x22,
	NVSEC_ATA_PWD_RAND = 0x36,
} sec_nvram_offset;

typedef enum : uint16_t
{
	NVFLAG_BOOT_TO_BOOTROM = 0x000c,
} nv_box_flag_mask;

void nvram_primary_test_print();

void nvram_primary_init();
void nvram_primary_close();
bool nvram_primary_enabled();
void nvram_primary_trans_start();
void nvram_primary_trans_restart();
bool nvram_primary_trans_insert(pri_nvram_setting_key setting_key, void* data, uint32_t length);
bool nvram_primary_trans_commit();
void* nvram_primary_read(pri_nvram_setting_key setting_key, uint32_t* length, bool use_persistant_settings);
bool nvram_primary_write(pri_nvram_setting_key setting_key, void* data, uint32_t length, bool use_persistant_settings);

void nvram_secondary_read(sec_nvram_offset offset, void* data, uint32_t length);
void nvram_secondary_write(sec_nvram_offset offset, void* data, uint32_t length);
bool get_box_flag(nv_box_flag_mask flag_mask);
void set_box_flag(nv_box_flag_mask flag_mask, bool enable);

#ifdef __cplusplus
}
#endif

#endif
