#include "../common/common.hpp"

#include <cassert>
#include <iostream>

namespace {
bool attached = true;
Result queryResult = 0;
Result openResult = 0;
Result metadataResult = 0;
int openCalls = 0;
int metadataCalls = 0;
int memoryWrites = 0;
DmntCheatProcessMetadata currentProcess{};
}

extern "C" Result dmntchtHasCheatProcess(bool* out) {
    *out = attached;
    return queryResult;
}

extern "C" Result dmntchtForceOpenCheatProcess() {
    ++openCalls;
    return openResult;
}

extern "C" Result dmntchtGetCheatProcessMetadata(DmntCheatProcessMetadata* out) {
    ++metadataCalls;
    *out = currentProcess;
    return metadataResult;
}

extern "C" Result dmntchtReadCheatProcessMemory(u64, void*, size_t) {
    assert(false && "These session tests must not read game memory");
    return 1;
}

extern "C" Result dmntchtWriteCheatProcessMemory(u64, const void*, size_t) {
    ++memoryWrites;
    return 0;
}

int main() {
    std::copy(feth::TARGET_BID.begin(), feth::TARGET_BID.end(), currentProcess.main_nso_build_id);
    currentProcess.process_id = 10;
    currentProcess.main_nso_extents.base = 0x100000;

    // Another tool has already attached before this overlay starts.
    assert(feth::gameIsRunning());
    assert(openCalls == 0 && metadataCalls == 1);
    assert(feth::s_processMetadata.process_id == 10);

    // Relaunching the same build must update its PID and memory base.
    currentProcess.process_id = 20;
    currentProcess.main_nso_extents.base = 0x200000;
    assert(feth::gameIsRunning());
    assert(feth::s_processMetadata.process_id == 20);
    assert(feth::s_processMetadata.main_nso_extents.base == 0x200000);

    attached = false;
    assert(feth::gameIsRunning());
    assert(openCalls == 1);

    openResult = 1;
    assert(!feth::gameIsRunning());
    attached = true;

    queryResult = 1;
    assert(!feth::gameIsRunning());
    queryResult = 0;
    metadataResult = 1;
    assert(!feth::gameIsRunning());
    metadataResult = 0;

    // Switching to an unsupported game must stop writes, despite cached metadata.
    currentProcess.main_nso_build_id[0] ^= 0xff;
    assert(!feth::gameIsRunning());
    feth::setItemsWithIdSet(nullptr, nullptr, nullptr, false);
    assert(memoryWrites == 0);

    std::cout << "Game session attachment, refresh, failures and build checks passed.\n";
}
