#include "pch.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU/BigArray.h"
#include "ACU_DefineNativeFunction.h"
#include "Common_Plugins/Common_PluginSide.h"

#include "Handles.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(VirtualForgesLog, "[AssetOverrides]");

#include "Mock_CombinedRawBufsReader.h"


DEFINE_GAME_FUNCTION(LinkedObjectDesc__Fill, 0x1426EBF50, int, __fastcall, (DatapackPackedObjectDesc* p_linkedObjInfo, DatapackReader* p_datapackReader));

std::optional<uint64> FindFirstHandleInDatapack(const fs::path& targetFilepath)
{
    /*
    Partially recreating a chain of actions that occur during the call to 142701730
    in order to retrieve the handles within the datapack.
    The recreated portion of logic is:
    within 142701730:
        14270174C calls 142764E30,      (initiate compressed file reader)
        then within 142704CE0:
            142704D44 calls 142747110   (initiate "gather information about datapack contents")
            142704D7A calls 1427665D0   (read 2 bytes == number of files within datapack)
            142704DBF calls 1426EBF50   (read info about next file in datapack)
    */
    std::ifstream infile = std::ifstream(targetFilepath, std::ios::binary);
    if (!infile) return {};
    infile.seekg(0, std::ios::end);
    uint32 filesize = (uint32)infile.tellg();
    infile.seekg(0);


    Mock_CombinedRawBufsReader reader;
    reader.m_File = std::move(infile);
    reader.Init_2(filesize);


    void* someAllocatorUsedDuringLoadDatapack = *(void**)0x1452585E0;
    void* theMoreCommonAllocator = *(void**)0x1452585D0;
    // Constructor called at 142704D44.
    DatapackReader retrieveDatapackStats(
        (CombinedRawBufsReader*)&reader,
        0, 1u, 0, 0x8000u, 0, 10, 0, theMoreCommonAllocator
    );

    // The game does not check for errors, so attempting to retrieve the handle from
    // an invalid datapack (e.g. just an empty file) will freeze the game.
    // I need to discern ahead of time the datapacks that will cause a freeze.
    // This criterium below seems to be working alright, though I don't know what it means.
    // UPD: Doesn't always work. For example, take the savegame file Save002.sav
    //      and change its extension to .data: this consistently crashes at startup.
    //      I don't want to reverse the compression algorithm, so I'll just try to make it clear
    //      in the instructions that the mod folders shouldn't contain junk.
    const bool isLooksLikeValidDatapack = retrieveDatapackStats.byte_89 == 9;
    if (!isLooksLikeValidDatapack)
    {
        LOG_DEBUG(VirtualForgesLog
            , "[error]\"%s\" does not look like a valid datapack.\n"
              "    The game will not do any error checking and can freeze, so I'm aborting now.\n"
            , targetFilepath.filename().u8string().c_str()
        );
        return {};
    }

    uint16 numFilesInDatapack = 0;
    retrieveDatapackStats.CastToReader().Unk050_Read2bytes(&numFilesInDatapack);

    if (!numFilesInDatapack) return {};
    DatapackPackedObjectDesc firstFileDesc;
    LinkedObjectDesc__Fill(&firstFileDesc, &retrieveDatapackStats);
    return firstFileDesc.handle;
    //std::vector<DatapackPackedObjectDesc> datapackContents;
    //datapackContents.resize(numFilesInDatapack);
    //for (uint16 i = 0; i < numFilesInDatapack; i++)
    //{
    //    LinkedObjectDesc__Fill(&datapackContents[i], &retrieveDatapackStats);
    //    LOG_DEBUG(VirtualForgesLog
    //        , L"[LoadDatapack]%3hu. %llu => %s"
    //        , i
    //        , datapackContents[i].handle
    //        , utf8_and_wide_string_conversion::utf8_decode(ACU::Handles::HandleToText(datapackContents[i].handle).c_str()).c_str()
    //    );
    //}
    //return datapackContents[0].handle;
}
