/*
 * Copyright (C) 2020-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/test/common/mocks/mock_driver_info.h"
#include "shared/test/common/os_interface/linux/sys_calls_linux_ult.h"
#include "shared/test/common/test_macros/test.h"

#include "level_zero/tools/source/sysman/diagnostics/linux/os_diagnostics_imp.h"
#include "level_zero/tools/source/sysman/events/linux/os_events_imp.h"
#include "level_zero/tools/source/sysman/firmware/linux/os_firmware_imp.h"
#include "level_zero/tools/source/sysman/ras/ras_imp.h"
#include "level_zero/tools/test/unit_tests/sources/sysman/linux/mock_sysman_fixture.h"

namespace NEO {
namespace SysCalls {
extern bool allowFakeDevicePath;
} // namespace SysCalls
} // namespace NEO

namespace L0 {
namespace ult {

inline static int mockAccessFailure(const char *pathname, int mode) {
    return -1;
}

inline static int mockAccessSuccess(const char *pathname, int mode) {
    return 0;
}

inline static int mockStatFailure(const char *pathname, struct stat *sb) noexcept {
    return -1;
}

inline static int mockStatSuccess(const char *pathname, struct stat *sb) noexcept {
    sb->st_mode = S_IWUSR | S_IRUSR;
    return 0;
}

inline static int mockStatNoPermissions(const char *pathname, struct stat *sb) noexcept {
    sb->st_mode = 0;
    return 0;
}

TEST_F(SysmanDeviceFixture, GivenValidDeviceHandleInSysmanImpCreationWhenAllSysmanInterfacesAreAssignedToNullThenExpectSysmanDeviceModuleContextsAreNull) {
    ze_device_handle_t hSysman = device->toHandle();
    SysmanDeviceImp *sysmanImp = new SysmanDeviceImp(hSysman);

    delete (sysmanImp->pPowerHandleContext);
    delete (sysmanImp->pFrequencyHandleContext);
    delete (sysmanImp->pFabricPortHandleContext);
    delete (sysmanImp->pTempHandleContext);
    delete (sysmanImp->pPci);
    delete (sysmanImp->pStandbyHandleContext);
    delete (sysmanImp->pEngineHandleContext);
    delete (sysmanImp->pSchedulerHandleContext);
    delete (sysmanImp->pRasHandleContext);
    delete (sysmanImp->pMemoryHandleContext);
    delete (sysmanImp->pGlobalOperations);
    delete (sysmanImp->pEvents);
    delete (sysmanImp->pFanHandleContext);
    delete (sysmanImp->pFirmwareHandleContext);
    delete (sysmanImp->pDiagnosticsHandleContext);
    delete (sysmanImp->pPerformanceHandleContext);
    delete (sysmanImp->pEcc);

    sysmanImp->pPowerHandleContext = nullptr;
    sysmanImp->pFrequencyHandleContext = nullptr;
    sysmanImp->pFabricPortHandleContext = nullptr;
    sysmanImp->pTempHandleContext = nullptr;
    sysmanImp->pPci = nullptr;
    sysmanImp->pStandbyHandleContext = nullptr;
    sysmanImp->pEngineHandleContext = nullptr;
    sysmanImp->pSchedulerHandleContext = nullptr;
    sysmanImp->pRasHandleContext = nullptr;
    sysmanImp->pMemoryHandleContext = nullptr;
    sysmanImp->pGlobalOperations = nullptr;
    sysmanImp->pEvents = nullptr;
    sysmanImp->pFanHandleContext = nullptr;
    sysmanImp->pFirmwareHandleContext = nullptr;
    sysmanImp->pDiagnosticsHandleContext = nullptr;
    sysmanImp->pPerformanceHandleContext = nullptr;
    sysmanImp->pEcc = nullptr;

    auto pLinuxSysmanImpTemp = static_cast<PublicLinuxSysmanImp *>(sysmanImp->pOsSysman);
    pLinuxSysmanImpTemp->pSysfsAccess = pSysfsAccess;
    pLinuxSysmanImpTemp->pProcfsAccess = pProcfsAccess;

    sysmanImp->init();
    // all sysman module contexts are null. Validating PowerHandleContext instead of all contexts
    EXPECT_EQ(sysmanImp->pPowerHandleContext, nullptr);
    pLinuxSysmanImpTemp->pSysfsAccess = nullptr;
    pLinuxSysmanImpTemp->pProcfsAccess = nullptr;
    delete sysmanImp;
    sysmanImp = nullptr;
}

TEST_F(SysmanDeviceFixture, GivenValidDeviceHandleAndIfSysmanDeviceInitFailsThenErrorReturnedWhileQueryingSysmanAPIs) {
    ze_device_handle_t hSysman = device->toHandle();
    auto pSysmanDeviceOriginal = static_cast<DeviceImp *>(device)->getSysmanHandle();

    // L0::SysmanDeviceHandleContext::init() would return nullptr as:
    // L0::SysmanDeviceHandleContext::init() --> sysmanDevice->init() --> pOsSysman->init() --> pSysfsAccess->getRealPath()
    // pSysfsAccess->getRealPath() would fail because pSysfsAccess is not mocked in this test case.
    auto pSysmanDeviceLocal = L0::SysmanDeviceHandleContext::init(hSysman);
    EXPECT_EQ(pSysmanDeviceLocal, nullptr);
    static_cast<DeviceImp *>(device)->setSysmanHandle(pSysmanDeviceLocal);
    uint32_t count = 0;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumSchedulers(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceProcessesGetState(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDevicePciGetBars(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumPowerDomains(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumFrequencyDomains(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumEngineGroups(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumStandbyDomains(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumFirmwares(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumMemoryModules(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumFabricPorts(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumTemperatureSensors(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumRasErrorSets(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumFans(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumDiagnosticTestSuites(hSysman, &count, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEnumPerformanceFactorDomains(hSysman, &count, nullptr));

    zes_device_properties_t properties;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceGetProperties(hSysman, &properties));
    zes_device_state_t state;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceGetState(hSysman, &state));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceReset(hSysman, true));
    zes_pci_properties_t pciProperties;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDevicePciGetProperties(hSysman, &pciProperties));
    zes_pci_state_t pciState;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDevicePciGetState(hSysman, &pciState));
    zes_pci_stats_t pciStats;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDevicePciGetStats(hSysman, &pciStats));
    zes_event_type_flags_t events = ZES_EVENT_TYPE_FLAG_DEVICE_DETACH;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEventRegister(hSysman, events));
    zes_pwr_handle_t phPower = {};
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceGetCardPowerDomain(hSysman, &phPower));
    ze_bool_t eccAvailable = false;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEccAvailable(device, &eccAvailable));
    ze_bool_t eccConfigurable = false;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceEccConfigurable(device, &eccConfigurable));
    zes_device_ecc_desc_t newState = {};
    zes_device_ecc_properties_t props = {};
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceSetEccState(device, &newState, &props));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zesDeviceGetEccState(device, &props));
    static_cast<DeviceImp *>(device)->setSysmanHandle(pSysmanDeviceOriginal);
}

using MockDeviceSysmanGetTest = Test<DeviceFixture>;
TEST_F(MockDeviceSysmanGetTest, GivenValidSysmanHandleSetInDeviceStructWhenGetThisSysmanHandleThenHandlesShouldBeSimilar) {
    SysmanDeviceImp *sysman = new SysmanDeviceImp(device->toHandle());
    device->setSysmanHandle(sysman);
    EXPECT_EQ(sysman, device->getSysmanHandle());
}

TEST_F(SysmanDeviceFixture, GivenValidDeviceHandleButSysmanInitFailsThenValidNullptrReceived) {
    ze_device_handle_t hSysman = device->toHandle();
    auto pSysmanDevice = L0::SysmanDeviceHandleContext::init(hSysman);
    EXPECT_EQ(pSysmanDevice, nullptr);
}

TEST_F(SysmanDeviceFixture, GivenSetValidDrmHandleForDeviceWhenDoingOsSysmanDeviceInitThenSameDrmHandleIsRetrieved) {
    EXPECT_EQ(&pLinuxSysmanImp->getDrm(), device->getOsInterface().getDriverModel()->as<Drm>());
}

TEST_F(SysmanDeviceFixture, GivenCreateFsAccessHandleWhenCallinggetFsAccessThenCreatedFsAccessHandleWillBeRetrieved) {
    if (pLinuxSysmanImp->pFsAccess != nullptr) {
        // delete previously allocated pFsAccess
        delete pLinuxSysmanImp->pFsAccess;
        pLinuxSysmanImp->pFsAccess = nullptr;
    }
    pLinuxSysmanImp->pFsAccess = FsAccess::create();
    EXPECT_EQ(&pLinuxSysmanImp->getFsAccess(), pLinuxSysmanImp->pFsAccess);
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingDirectoryExistsWithValidAndInvalidPathThenSuccessAndFailureAreReturnedRespectively) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->accessSyscall = mockAccessSuccess;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_TRUE(tempFsAccess->directoryExists(path));
    tempFsAccess->accessSyscall = mockAccessFailure;
    path = "invalidDiretory";
    EXPECT_FALSE(tempFsAccess->directoryExists(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicSysfsAccessClassWhenCallingDirectoryExistsWithInvalidPathThenFalseIsRetured) {
    PublicFsAccess *tempSysfsAccess = new PublicFsAccess();
    tempSysfsAccess->accessSyscall = mockAccessFailure;
    std::string path = "invalidDiretory";
    EXPECT_FALSE(tempSysfsAccess->directoryExists(path));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanWriteWithUserHavingWritePermissionsThenSuccessIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatSuccess;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_SUCCESS, tempFsAccess->canWrite(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanReadWithUserHavingReadPermissionsThenSuccessIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatSuccess;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_SUCCESS, tempFsAccess->canRead(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanWriteWithUserNotHavingWritePermissionsThenInsufficientIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatNoPermissions;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS, tempFsAccess->canWrite(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanReadWithUserNotHavingReadPermissionsThenInsufficientIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatNoPermissions;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS, tempFsAccess->canRead(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanReadWithInvalidPathThenErrorIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatFailure;
    std::string path = "invalidPath";
    EXPECT_EQ(ZE_RESULT_ERROR_UNKNOWN, tempFsAccess->canRead(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenPublicFsAccessClassWhenCallingCanWriteWithInvalidPathThenErrorIsReturned) {
    PublicFsAccess *tempFsAccess = new PublicFsAccess();
    tempFsAccess->statSyscall = mockStatFailure;
    std::string path = "invalidPath";
    EXPECT_EQ(ZE_RESULT_ERROR_UNKNOWN, tempFsAccess->canRead(path));
    delete tempFsAccess;
}

TEST_F(SysmanDeviceFixture, GivenValidPathnameWhenCallingFsAccessExistsThenSuccessIsReturned) {
    VariableBackup<bool> allowFakeDevicePathBackup(&SysCalls::allowFakeDevicePath, true);
    auto fsAccess = pLinuxSysmanImp->getFsAccess();

    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_TRUE(fsAccess.fileExists(path));
}

TEST_F(SysmanDeviceFixture, GivenInvalidPathnameWhenCallingFsAccessExistsThenErrorIsReturned) {
    auto fsAccess = pLinuxSysmanImp->getFsAccess();

    std::string path = "noSuchFileOrDirectory";
    EXPECT_FALSE(fsAccess.fileExists(path));
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessAndValidDeviceNameWhenCallingBindDeviceThenSuccessIsReturned) {

    std::string deviceName = "card0";

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        std::string deviceName = "card0";
        return static_cast<ssize_t>(deviceName.size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->bindDevice(deviceName));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessAndValidDeviceNameWhenCallingUnbindDeviceThenSuccessIsReturned) {

    std::string deviceName = "card0";

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        std::string deviceName = "card0";
        return static_cast<ssize_t>(deviceName.size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->unbindDevice(deviceName));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenValidPathnameWhenCallingSysfsAccessGetFileModeThenSuccessIsReturned) {
    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    char cwd[PATH_MAX];
    ::mode_t mode;
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->getFileMode(path, mode));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassWhenCallingCanWriteWithUserHavingWritePermissionsThenSuccessIsReturned) {
    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    tempSysfsAccess->statSyscall = mockStatSuccess;
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->canWrite(path));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassWhenCallingCanReadWithInvalidPathThenErrorIsReturned) {
    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    tempSysfsAccess->statSyscall = mockStatFailure;
    std::string path = "invalidPath";
    EXPECT_EQ(ZE_RESULT_ERROR_UNKNOWN, tempSysfsAccess->canRead(path));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenValidPathnameWhenCallingSysfsAccessExistsThenSuccessIsReturned) {
    VariableBackup<bool> allowFakeDevicePathBackup(&SysCalls::allowFakeDevicePath, true);
    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    EXPECT_TRUE(tempSysfsAccess->fileExists(path));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndValidDirectoryWhenCallingscanDirEntriesThenSuccessIsReturned) {
    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    char cwd[PATH_MAX];
    std::string path = getcwd(cwd, PATH_MAX);
    std::vector<std::string> dir;
    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->scanDirEntries(path, dir));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndConstantStringWhenCallingWriteThenSuccessIsReturned) {
    const std::string fileName = "mockFile.txt";
    const std::string str = "Mock String";

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        const std::string str = "Mock String";
        return static_cast<ssize_t>(str.size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->write(fileName, str));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndConstantIntegerWhenCallingWriteThenSuccessIsReturned) {

    const std::string fileName = "mockFile.txt";
    const int iVal32 = 0;

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        const int iVal32 = 0;
        std::ostringstream stream;
        stream << iVal32;
        return static_cast<ssize_t>(stream.str().size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->write(fileName, iVal32));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndIntegerWhenCallingReadThenSuccessIsReturned) {

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPread)> mockPread(&NEO::SysCalls::sysCallsPread, [](int fd, void *buf, size_t count, off_t offset) -> ssize_t {
        std::string value = "123";
        memcpy(buf, value.data(), value.size());
        return value.size();
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    const std::string fileName = "mockFile.txt";
    int iVal32;

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->read(fileName, iVal32));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndUnsignedIntegerWhenCallingReadThenSuccessIsReturned) {

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPread)> mockPread(&NEO::SysCalls::sysCallsPread, [](int fd, void *buf, size_t count, off_t offset) -> ssize_t {
        std::string value = "123";
        memcpy(buf, value.data(), value.size());
        return value.size();
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    const std::string fileName = "mockFile.txt";
    uint32_t uVal32;

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->read(fileName, uVal32));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndConstantDoubleValueWhenCallingWriteThenSuccessIsReturned) {

    const std::string fileName = "mockFile.txt";
    const double dVal = 0.0;

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        const double dVal = 0.0;
        std::ostringstream stream;
        stream << dVal;
        return static_cast<ssize_t>(stream.str().size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->write(fileName, dVal));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndDoubleWhenCallingReadThenSuccessIsReturned) {

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPread)> mockPread(&NEO::SysCalls::sysCallsPread, [](int fd, void *buf, size_t count, off_t offset) -> ssize_t {
        std::string value = "123";
        memcpy(buf, value.data(), value.size());
        return value.size();
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    const std::string fileName = "mockFile.txt";
    double dVal;

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->read(fileName, dVal));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndUnsignedLongValueWhenCallingWriteThenSuccessIsReturned) {
    const std::string fileName = "mockFile.txt";
    const uint64_t uVal64 = 0;

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPwrite)> mockPwrite(&NEO::SysCalls::sysCallsPwrite, [](int fd, const void *buf, size_t count, off_t offset) -> ssize_t {
        const uint64_t uVal64 = 0;
        std::ostringstream stream;
        stream << uVal64;
        return static_cast<ssize_t>(stream.str().size());
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->write(fileName, uVal64));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenSysfsAccessClassAndUnsignedLongWhenCallingReadThenSuccessIsReturned) {

    VariableBackup<decltype(NEO::SysCalls::sysCallsOpen)> mockOpen(&NEO::SysCalls::sysCallsOpen, [](const char *pathname, int flags) -> int {
        return 1;
    });

    VariableBackup<decltype(NEO::SysCalls::sysCallsPread)> mockPread(&NEO::SysCalls::sysCallsPread, [](int fd, void *buf, size_t count, off_t offset) -> ssize_t {
        std::string value = "123";
        memcpy(buf, value.data(), value.size());
        return value.size();
    });

    PublicSysfsAccess *tempSysfsAccess = new PublicSysfsAccess();
    const std::string fileName = "mockFile.txt";
    uint64_t uVal64;

    EXPECT_EQ(ZE_RESULT_SUCCESS, tempSysfsAccess->read(fileName, uVal64));
    delete tempSysfsAccess;
}

TEST_F(SysmanDeviceFixture, GivenCreateSysfsAccessHandleWhenCallinggetSysfsAccessThenCreatedSysfsAccessHandleHandleWillBeRetrieved) {
    if (pLinuxSysmanImp->pSysfsAccess != nullptr) {
        // delete previously allocated pSysfsAccess
        delete pLinuxSysmanImp->pSysfsAccess;
        pLinuxSysmanImp->pSysfsAccess = nullptr;
    }
    pLinuxSysmanImp->pSysfsAccess = SysfsAccess::create("");
    EXPECT_EQ(&pLinuxSysmanImp->getSysfsAccess(), pLinuxSysmanImp->pSysfsAccess);
}

TEST_F(SysmanDeviceFixture, GivenValidPidWhenCallingProcfsAccessGetFileDescriptorsThenSuccessIsReturned) {
    auto procfsAccess = pLinuxSysmanImp->getProcfsAccess();

    ::pid_t processID = getpid();
    std::vector<int> listFiles;
    EXPECT_EQ(ZE_RESULT_SUCCESS, procfsAccess.getFileDescriptors(processID, listFiles));
}

TEST_F(SysmanDeviceFixture, GivenValidProcfsAccessHandleWhenCallingListProcessesThenSuccessIsReturned) {
    auto procfsAccess = pLinuxSysmanImp->getProcfsAccess();

    std::vector<::pid_t> listPid;
    EXPECT_EQ(ZE_RESULT_SUCCESS, procfsAccess.listProcesses(listPid));
}

TEST_F(SysmanDeviceFixture, GivenValidProcfsAccessHandleAndKillProcessWhenCallingIsAliveThenErrorIsReturned) {
    auto procfsAccess = pLinuxSysmanImp->getProcfsAccess();
    int pid = NEO::SysCalls::getProcessId();
    procfsAccess.kill(pid);
    EXPECT_FALSE(procfsAccess.isAlive(pid));
}

TEST_F(SysmanDeviceFixture, GivenCreateProcfsAccessHandleWhenCallinggetProcfsAccessThenCreatedProcfsAccessHandleWillBeRetrieved) {
    if (pLinuxSysmanImp->pProcfsAccess != nullptr) {
        // delete previously allocated pProcfsAccess
        delete pLinuxSysmanImp->pProcfsAccess;
        pLinuxSysmanImp->pProcfsAccess = nullptr;
    }
    pLinuxSysmanImp->pProcfsAccess = ProcfsAccess::create();
    EXPECT_EQ(&pLinuxSysmanImp->getProcfsAccess(), pLinuxSysmanImp->pProcfsAccess);
}

TEST_F(SysmanDeviceFixture, GivenValidPidWhenCallingProcfsAccessIsAliveThenSuccessIsReturned) {
    VariableBackup<bool> allowFakeDevicePathBackup(&SysCalls::allowFakeDevicePath, true);
    auto procfsAccess = pLinuxSysmanImp->getProcfsAccess();

    EXPECT_TRUE(procfsAccess.isAlive(getpid()));
}

TEST_F(SysmanDeviceFixture, GivenInvalidPidWhenCallingProcfsAccessIsAliveThenErrorIsReturned) {
    auto procfsAccess = pLinuxSysmanImp->getProcfsAccess();

    EXPECT_FALSE(procfsAccess.isAlive(reinterpret_cast<::pid_t>(-1)));
}

TEST_F(SysmanDeviceFixture, GivenValidDeviceHandleThenSameHandleIsRetrievedFromOsSpecificCode) {
    EXPECT_EQ(pLinuxSysmanImp->getDeviceHandle(), device);
}

TEST_F(SysmanDeviceFixture, GivenPmuInterfaceHandleWhenCallinggetPmuInterfaceThenCreatedPmuInterfaceHandleWillBeRetrieved) {
    if (pLinuxSysmanImp->pPmuInterface != nullptr) {
        // delete previously allocated pPmuInterface
        delete pLinuxSysmanImp->pPmuInterface;
        pLinuxSysmanImp->pPmuInterface = nullptr;
    }
    pLinuxSysmanImp->pPmuInterface = PmuInterface::create(pLinuxSysmanImp);
    EXPECT_EQ(pLinuxSysmanImp->getPmuInterface(), pLinuxSysmanImp->pPmuInterface);
}

TEST_F(SysmanDeviceFixture, GivenValidPciPathWhileGettingCardBusPortThenReturnedPathIs1LevelUpThenTheCurrentPath) {
    const std::string mockBdf = "0000:00:02.0";
    const std::string mockRealPath = "/sys/devices/pci0000:00/0000:00:01.0/0000:01:00.0/0000:02:01.0/" + mockBdf;
    const std::string mockRealPath1LevelUp = "/sys/devices/pci0000:00/0000:00:01.0/0000:01:00.0";

    std::string pciRootPort1 = pLinuxSysmanImp->getPciCardBusDirectoryPath(mockRealPath);
    EXPECT_EQ(pciRootPort1, mockRealPath1LevelUp);

    std::string pciRootPort2 = pLinuxSysmanImp->getPciCardBusDirectoryPath("device");
    EXPECT_EQ(pciRootPort2, "device");
}

TEST_F(SysmanDeviceFixture, GivenNullDrmHandleWhenGettingDrmHandleThenValidDrmHandleIsReturned) {
    pLinuxSysmanImp->releaseLocalDrmHandle();
    EXPECT_NO_THROW(pLinuxSysmanImp->getDrm());
}

TEST_F(SysmanDeviceFixture, GivenValidDeviceHandleWhenGettingFwUtilInterfaceAndGetPciBdfFailsThenFailureIsReturned) {
    auto deviceImp = static_cast<L0::DeviceImp *>(pLinuxSysmanImp->getDeviceHandle());

    deviceImp->driverInfo.reset(nullptr);
    FirmwareUtil *pFwUtilInterfaceOld = pLinuxSysmanImp->pFwUtilInterface;
    pLinuxSysmanImp->pFwUtilInterface = nullptr;

    EXPECT_EQ(pLinuxSysmanImp->getFwUtilInterface(), nullptr);
    pLinuxSysmanImp->pFwUtilInterface = pFwUtilInterfaceOld;
}

TEST_F(SysmanDeviceFixture, GivenValidEnumeratedHandlesWhenReleaseIsCalledThenHandleCountZeroIsReturned) {
    uint32_t count = 0;

    const std::vector<std::string> mockSupportedDiagTypes = {"MOCKSUITE1", "MOCKSUITE2"};
    std::vector<std::string> mockSupportedFirmwareTypes = {"GSC", "OptionROM", "PSC"};

    FirmwareImp *ptestFirmwareImp = new FirmwareImp(pSysmanDeviceImp->pFirmwareHandleContext->pOsSysman, mockSupportedFirmwareTypes[0]);
    pSysmanDeviceImp->pFirmwareHandleContext->handleList.push_back(ptestFirmwareImp);
    count = 0;
    ze_result_t result = zesDeviceEnumFirmwares(device->toHandle(), &count, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 1u);

    count = 0;
    DiagnosticsImp *ptestDiagnosticsImp = new DiagnosticsImp(pSysmanDeviceImp->pDiagnosticsHandleContext->pOsSysman, mockSupportedDiagTypes[0]);
    pSysmanDeviceImp->pDiagnosticsHandleContext->handleList.push_back(ptestDiagnosticsImp);
    result = zesDeviceEnumDiagnosticTestSuites(device->toHandle(), &count, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 1u);

    count = 0;
    RasImp *pRas = new RasImp(pSysmanDeviceImp->pRasHandleContext->pOsSysman, ZES_RAS_ERROR_TYPE_CORRECTABLE, device->toHandle());
    pSysmanDeviceImp->pRasHandleContext->handleList.push_back(pRas);
    result = zesDeviceEnumRasErrorSets(device->toHandle(), &count, NULL);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 3u);

    pLinuxSysmanImp->releaseSysmanDeviceResources();

    count = 0;
    result = zesDeviceEnumFirmwares(device->toHandle(), &count, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 0u);

    count = 0;
    result = zesDeviceEnumDiagnosticTestSuites(device->toHandle(), &count, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 0u);

    count = 0;
    result = zesDeviceEnumRasErrorSets(device->toHandle(), &count, NULL);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(count, 0u);
}

TEST_F(SysmanDeviceFixture, GivenDriverEventsUtilAsNullWhenSysmanDriverDestructorIsCalledThenVerifyNoExceptionOccured) {
    VariableBackup<L0::OsSysmanDriver *> driverBackup(&GlobalOsSysmanDriver);
    auto pPublicLinuxSysmanDriverImp = new PublicLinuxSysmanDriverImp();
    GlobalOsSysmanDriver = static_cast<L0::OsSysmanDriver *>(pPublicLinuxSysmanDriverImp);

    auto pUdevLib = new UdevLibMock();
    pPublicLinuxSysmanDriverImp->pUdevLib = pUdevLib;

    delete pPublicLinuxSysmanDriverImp->pLinuxEventsUtil;
    pPublicLinuxSysmanDriverImp->pLinuxEventsUtil = nullptr;
    EXPECT_NO_THROW(L0::osSysmanDriverDestructor());
}

TEST_F(SysmanMultiDeviceFixture, GivenValidDeviceHandleHavingSubdevicesWhenValidatingSysmanHandlesForSubdevicesThenSysmanHandleForSubdeviceWillBeSameAsSysmanHandleForDevice) {
    ze_device_handle_t hSysman = device->toHandle();
    auto pSysmanDeviceOriginal = static_cast<DeviceImp *>(device)->getSysmanHandle();
    auto pSysmanDeviceLocal = L0::SysmanDeviceHandleContext::init(hSysman);
    EXPECT_EQ(pSysmanDeviceLocal, nullptr);
    static_cast<DeviceImp *>(device)->setSysmanHandle(pSysmanDeviceLocal);

    uint32_t count = 0;
    EXPECT_EQ(ZE_RESULT_SUCCESS, device->getSubDevices(&count, nullptr));
    std::vector<ze_device_handle_t> subDeviceHandles(count, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, device->getSubDevices(&count, subDeviceHandles.data()));
    for (auto subDeviceHandle : subDeviceHandles) {
        L0::DeviceImp *subDeviceHandleImp = static_cast<DeviceImp *>(Device::fromHandle(subDeviceHandle));
        EXPECT_EQ(subDeviceHandleImp->getSysmanHandle(), device->getSysmanHandle());
    }
    static_cast<DeviceImp *>(device)->setSysmanHandle(pSysmanDeviceOriginal);
}

TEST_F(SysmanMultiDeviceFixture, GivenValidEffectiveUserIdCheckWhetherPermissionsReturnedByIsRootUserAreCorrect) {
    int euid = geteuid();
    auto pFsAccess = pLinuxSysmanImp->getFsAccess();
    if (euid == 0) {
        EXPECT_EQ(true, pFsAccess.isRootUser());
    } else {
        EXPECT_EQ(false, pFsAccess.isRootUser());
    }
}

TEST_F(SysmanMultiDeviceFixture, GivenSysmanEnvironmentVariableSetWhenCreateL0DeviceThenSysmanHandleCreateIsAttempted) {
    driverHandle->enableSysman = true;
    // In SetUp of SysmanMultiDeviceFixture, sysman handle for device is already created, so new sysman handle should not be created
    static_cast<DeviceImp *>(device)->createSysmanHandle(true);
    EXPECT_EQ(device->getSysmanHandle(), pSysmanDevice);

    static_cast<DeviceImp *>(device)->createSysmanHandle(false);
    EXPECT_EQ(device->getSysmanHandle(), pSysmanDevice);

    // delete previously allocated sysman handle and then attempt to create sysman handle again
    delete pSysmanDevice;
    device->setSysmanHandle(nullptr);
    static_cast<DeviceImp *>(device)->createSysmanHandle(true);
    EXPECT_EQ(device->getSysmanHandle(), nullptr);

    static_cast<DeviceImp *>(device)->createSysmanHandle(false);
    EXPECT_EQ(device->getSysmanHandle(), nullptr);
}

class UnknownDriverModel : public DriverModel {
  public:
    UnknownDriverModel() : DriverModel(DriverModelType::UNKNOWN) {}
    void setGmmInputArgs(void *args) override {}
    uint32_t getDeviceHandle() const override { return 0u; }
    PhysicalDevicePciBusInfo getPciBusInfo() const override {
        PhysicalDevicePciBusInfo pciBusInfo(PhysicalDevicePciBusInfo::invalidValue, PhysicalDevicePciBusInfo::invalidValue, PhysicalDevicePciBusInfo::invalidValue, PhysicalDevicePciBusInfo::invalidValue);
        return pciBusInfo;
    }
    PhysicalDevicePciSpeedInfo getPciSpeedInfo() const override { return {}; }

    bool isGpuHangDetected(OsContext &osContext) override {
        return false;
    }
};

using SysmanUnknownDriverModelTest = Test<DeviceFixture>;
TEST_F(SysmanUnknownDriverModelTest, GivenDriverModelTypeIsNotDrmWhenExecutingSysmanOnLinuxThenErrorIsReturned) {
    neoDevice->getExecutionEnvironment()->rootDeviceEnvironments[device->getRootDeviceIndex()]->osInterface = std::make_unique<NEO::OSInterface>();
    auto &osInterface = device->getOsInterface();
    osInterface.setDriverModel(std::make_unique<UnknownDriverModel>());
    auto pSysmanDeviceImp = std::make_unique<SysmanDeviceImp>(device->toHandle());
    auto pLinuxSysmanImp = static_cast<PublicLinuxSysmanImp *>(pSysmanDeviceImp->pOsSysman);
    EXPECT_EQ(ZE_RESULT_ERROR_UNSUPPORTED_FEATURE, pLinuxSysmanImp->init());
}

} // namespace ult
} // namespace L0
