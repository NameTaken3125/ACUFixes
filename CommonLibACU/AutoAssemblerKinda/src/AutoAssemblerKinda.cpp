
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
// Windows Header Files
#include <windows.h>

#include <sstream>

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

using AutoAssemblerKinda::byte;


SYSTEM_INFO& GetCachedSystemInfo()
{
    static SYSTEM_INFO sysinfo = []() {SYSTEM_INFO sysinfo; GetSystemInfo(&sysinfo); return sysinfo; }();
    return sysinfo;
}
#define dprintf(...)
// From `<Cheat Engine source>/Cheat Engine/memoryquery.pas`, Windows version.
void* FindFreeBlockForRegion(uintptr_t base, unsigned int size)
{
    using ptrUint = uintptr_t;
    using ptrInt = intptr_t;
    SYSTEM_INFO& systeminfo = GetCachedSystemInfo();

    MEMORY_BASIC_INFORMATION mbi;
    ptrUint x;
    ptrUint offset;
    ptrUint b, oldb;

    ptrUint minAddress, maxAddress;

    void* result = nullptr;
    if (base == 0) { return nullptr; }
    if (base > 0x70000000) {
        minAddress = base - 0x70000000;
    }
    else {
        minAddress = 0x10000;
    }
    if (base + 0x70000000 > base) {
        maxAddress = base + 0x70000000;
    }
    else {
        maxAddress = ptrUint(0xffffffffffff0000);
    }

    constexpr bool processhandler_is64bit = true;
    if (processhandler_is64bit)
    {
        if (minAddress > uintptr_t(systeminfo.lpMaximumApplicationAddress) || minAddress < uintptr_t(systeminfo.lpMinimumApplicationAddress))
            minAddress = uintptr_t(systeminfo.lpMinimumApplicationAddress);
        if (maxAddress > uintptr_t(systeminfo.lpMaximumApplicationAddress) || maxAddress < uintptr_t(systeminfo.lpMinimumApplicationAddress))
            maxAddress = uintptr_t(systeminfo.lpMaximumApplicationAddress);
    }
    dprintf("minaddress=%.16llx", minAddress);
    dprintf("maxaddress=%.16llx", maxAddress);

    b = minAddress;

    ZeroMemory(&mbi, sizeof(mbi));
    while (VirtualQuery((LPCVOID)b, &mbi, sizeof(mbi)) == sizeof(mbi))
    {
        dprintf("  +VQ: allocBase:%llx, pageBase: %llx, regionSize:%x, state: %x ", uintptr_t(mbi.AllocationBase), mbi.BaseAddress, mbi.RegionSize, mbi.State);
        if ((ptrUint)mbi.BaseAddress > maxAddress) { return nullptr; } //no memory found, just return 0 and let windows decide
        if (mbi.State == MEM_FREE && mbi.RegionSize > size)
        {
            if ((ptrUint)mbi.BaseAddress % systeminfo.dwAllocationGranularity > 0)
            {
                //the whole size can not be used
                x = ptrUint(mbi.BaseAddress);
                offset = systeminfo.dwAllocationGranularity - (x % systeminfo.dwAllocationGranularity);
                //check if there's enough left
                if (mbi.RegionSize - offset >= size)
                {
                    //yes
                    x = x + offset;
                    if (x < base)
                    {
                        x = x + (mbi.RegionSize - offset) - size;
                        if (x > base) { x = base; }
                        //now decrease x till it's alligned properly
                        x = x - (x % systeminfo.dwAllocationGranularity);
                    }
                    //if the difference is closer then use that
                    if (result == nullptr) {
                        result = (void*)x;
                        dprintf("1 result: %llx", result);
                    }
                    else
                    {
                        if (abs(ptrInt(x - base)) < abs(ptrInt(ptrUint(result) - base))) {
                            result = (void*)x;
                            dprintf("2 result: %llx", result);
                        }
                    }
                }
                //nope
            }
            else
            {
                x = ptrUint(mbi.BaseAddress);
                if (x < base)
                {
                    //try to get it the closest possible (so to the end of the region-size and aligned by dwAllocationGranularity)
                    x = (x + mbi.RegionSize) - size;
                    if (x > base) { x = base; }

                    //now decrease x till it's alligned properly
                    // Round down to a multiple of `systeminfo.dwAllocationGranularity`.
                    x = x - (x % systeminfo.dwAllocationGranularity);
                }
                if (result == nullptr) {
                    result = (void*)x;
                    dprintf("3 result: %llx", result);
                }
                else
                {
                    if (abs(ptrInt(x - base)) < abs(ptrInt(ptrUint(result) - base))) {
                        result = (void*)x;
                        dprintf("4 result: %llx", result);
                    }
                }
            }
        }
        // Round RegionSize up to a multiple of `systeminfo.dwAllocationGranularity`.
        if (mbi.RegionSize % systeminfo.dwAllocationGranularity > 0) {
            mbi.RegionSize = mbi.RegionSize + (systeminfo.dwAllocationGranularity - (mbi.RegionSize % systeminfo.dwAllocationGranularity));
        }
        oldb = b;
        b = ptrUint(mbi.BaseAddress) + mbi.RegionSize;

        if (b > maxAddress) {
            dprintf("b>maxAddress; result: %llx, b: %llx, oldb: %llx", result, b, oldb);
            return result;
        }
        if (oldb > b) {
            dprintf("overflow; b: %llx, oldb: %llx", b, oldb);
            return result; //overflow
        }
    }
    dprintf("returning; result: %llx, b: %llx, oldb: %llx", result, b, oldb);
    return result;
}
void PatchMemory(void* dst, SIZE_T size, const void* src)
{
    DWORD oldProtect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void WriteableSymbol::Write()
{
    if (m_resultantCode.empty()) { return; }
    PatchMemory((void*)m_ResolvedAddr.value(), m_resultantCode.size(), m_resultantCode.data());
}
void StaticSymbol::Write()
{
    if (m_resultantCode.empty()) { return; }
    m_OriginalBytes.resize(m_resultantCode.size(), 0);
    memcpy(&m_OriginalBytes[0], (void*)m_ResolvedAddr.value(), m_resultantCode.size());
    PatchMemory((void*)m_ResolvedAddr.value(), m_resultantCode.size(), m_resultantCode.data());
}
void StaticSymbol::Unwrite()
{
    if (m_resultantCode.empty()) { return; }
    PatchMemory((void*)m_ResolvedAddr.value(), m_OriginalBytes.size(), m_OriginalBytes.data());
}
AllocatedWriteableSymbol::~AllocatedWriteableSymbol()
{
    if (m_SuccessfulAllocBase)
    {
        VirtualFree((void*)m_SuccessfulAllocBase.value(), 0, MEM_RELEASE);
    }
}
void AssemblerContext::WriteChanges()
{
    for (auto& allocSymbol : m_AllocsQuickAccess)
    {
        allocSymbol->Write();
    }
    for (StaticSymbol* define : m_DefinesQuickAccess)
    {
        define->Write();
    }
}
void AssemblerContext::Unwrite()
{
    for (StaticSymbol* define : m_DefinesQuickAccess)
    {
        define->Unwrite();
    }
}
SymbolWithAnAddress* AssemblerContext::GetSymbol(const std::string_view& symbolName)
{
    for (auto& symbol : m_Symbols) {
        if (symbol->m_SymbolName == symbolName) {
            return symbol.get();
        }
    }
    return nullptr;
}
StaticSymbol& AssemblerContext::MakeNew_Define(uintptr_t addr, const std::string_view& symbolName)
{
    auto& newItem = m_Symbols.emplace_back(std::make_unique<StaticSymbol>(addr, this));
    newItem->m_SymbolName = symbolName;
    StaticSymbol* asDef = static_cast<StaticSymbol*>(newItem.get());
    m_DefinesQuickAccess.push_back(asDef);
    return *asDef;
}
Label& AssemblerContext::MakeNew_Label(const std::string_view& symbolName)
{
    auto& newItem = m_Symbols.emplace_back(std::make_unique<Label>(std::optional<uintptr_t>()));
    newItem->m_SymbolName = symbolName;
    Label* asLabel = static_cast<Label*>(newItem.get());
    m_LabelsQuickAccess.push_back(asLabel);
    return *asLabel;
}
AllocatedWriteableSymbol& AssemblerContext::MakeNew_Alloc(uint32_t size, const std::string_view& symbolName, uintptr_t preferredAddr)
{
    auto& newItem = m_Symbols.emplace_back(std::make_unique<AllocatedWriteableSymbol>(std::optional<uintptr_t>(), this));
    newItem->m_SymbolName = symbolName;
    AllocatedWriteableSymbol* asAlloc = static_cast<AllocatedWriteableSymbol*>(newItem.get());
    asAlloc->m_SizeToAllocate = size;
    asAlloc->m_PreferredAddr = preferredAddr;
    m_AllocsQuickAccess.push_back(asAlloc);
    return *asAlloc;
}
void AssemblerContext::AddSymbolRef(SymbolWithAnAddress& symbol, const SymbolMention& newRef)
{
    symbol.m_ReferencesToResolve.push_back(newRef);
}
void AssemblerContext::AssignLabel(Label& label, WriteableSymbol& assignedInWhichSymbol, uint32_t offset)
{
    label.m_AssignedInWhichSymbol = &assignedInWhichSymbol;
    label.m_Offset = offset;
}
ByteVector SymbolWithAnAddress::CopyCurrentBytes(size_t numBytes)
{
    ByteVector result;
    std::vector<byte>& copiedBytes = result.m_bytes;
    result.m_bytes.resize(numBytes);
    uintptr_t readFrom = m_ResolvedAddr.value();
    DWORD oldProtect;
    VirtualProtect((void*)readFrom, numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(&copiedBytes[0], (void*)readFrom, numBytes);
    VirtualProtect((void*)readFrom, numBytes, oldProtect, &oldProtect);
    return std::move(result);
}
void WriteableSymbol::ProcessNewCodeElements(size_t idxToStartProcessingFrom)
{
    using _2bytes_t = byte[2];
    using _4bytes_t = byte[4];
    using _8bytes_t = byte[8];
    m_resultantCode.reserve(m_codeElements.size() * 4);
    for (auto i = idxToStartProcessingFrom; i < m_codeElements.size(); i++)
    {
        CodeElement& block = m_codeElements[i];
        if (byte* bt = std::get_if<byte>(&block)) {
            m_resultantCode.push_back(*bt);
        }
        else if (db* data = std::get_if<db>(&block)) {
            for (auto& b : data->m_Values)
            {
                m_resultantCode.push_back(b);
            }
        }
        else if (dw* data = std::get_if<dw>(&block)) {
            for (auto& w : data->m_Values) {
                for (byte b : (_2bytes_t&)w) {
                    m_resultantCode.push_back(b);
                }
            }
        }
        else if (dd* data = std::get_if<dd>(&block)) {
            for (auto& d : data->m_Values) {
                for (byte b : (_4bytes_t&)d) {
                    m_resultantCode.push_back(b);
                }
            }
        }
        else if (dq* data = std::get_if<dq>(&block)) {
            for (auto& q : data->m_Values) {
                for (byte b : (_8bytes_t&)q) {
                    m_resultantCode.push_back(b);
                }
            }
        }
        else if (nop* nops = std::get_if<nop>(&block)) {
            for (size_t i = 0; i < nops->m_HowManyBytes; i++) {
                m_resultantCode.push_back(0x90);
            }
        }
        else if (std::string_view* hexString = std::get_if<std::string_view>(&block))
        {
            // Repeatedly:
            // - Skip all whitespace,
            // - Read 2 characters (at most),
            // - Attempt to convert them to a byte.
            std::stringstream hexss{ hexString->data() };
            hexss.setf(hexss.skipws);
            std::string _2byteBuf;
            byte parsedByte;
            while (true) {
                hexss.width(2);
                hexss >> _2byteBuf;
                if (hexss.eof()) {
                    break;
                }
                if (sscanf_s(_2byteBuf.c_str(), "%02hhx", &parsedByte) == 1) {
                    // Byte has been parsed.
                    m_resultantCode.push_back(parsedByte);
                }
            }
        }
        else if (RIP* relAddr = std::get_if<RIP>(&block)) {
            m_ctx->AddSymbolRef(relAddr->m_Symbol, SymbolMention(*this, (int)m_resultantCode.size(), SymbolMentionTypeRelative{ relAddr->m_HowManyBytesUntilTheEndOfOpcodeIncludingThese4bytes }));
            _4bytes_t relativeAddrPlaceholderZeros = { 0 };
            for (byte b : relativeAddrPlaceholderZeros) {
                m_resultantCode.push_back(b);
            }
        }
        else if (ABS* absAddr = std::get_if<ABS>(&block)) {
            m_ctx->AddSymbolRef(absAddr->m_Symbol, SymbolMention(*this, (int)m_resultantCode.size(), SymbolMentionTypeAbsolute{ absAddr->m_AbsoluteAddrWidth }));
            for (size_t i = 0; i < absAddr->m_AbsoluteAddrWidth; i++) {
                m_resultantCode.push_back(0);
            }
        }
        else if (LabelPlacement* labelAssignment = std::get_if<LabelPlacement>(&block)) {
            m_ctx->AssignLabel(labelAssignment->m_Label, *this, (uint32_t)m_resultantCode.size());
        }
    }
}
void WriteableSymbol::SetCodeElements(const std::vector<CodeElement>& codeElements)
{
    m_codeElements = codeElements;
    ProcessNewCodeElements(0);
}
void WriteableSymbol::AppendCodeElements(const std::vector<CodeElement>& codeElements)
{
    size_t firstNewElementIdx = m_codeElements.size();
    m_codeElements.reserve(m_codeElements.size() + codeElements.size() * 4);
    for (auto& codeElem : codeElements) {
        m_codeElements.push_back(codeElem);
    }
    ProcessNewCodeElements(firstNewElementIdx);
}
std::string WriteableSymbol::GetResultBytesString()
{
    std::stringstream ss;
    ss << std::hex;
    for (byte b : m_resultantCode)
    {
        ss.width(2);
        ss.fill('0');
        ss << (unsigned short)b << ' ';
    }
    return ss.str();
}


void AssemblerContext::AllocateVariables()
{
    for (auto& currentAlloc : m_AllocsQuickAccess)
    {
        void* allocBase = nullptr;
        // 10 tries is a very simplified version of what Cheat Engine itself is doing
        // (<Cheat Engine source code>/CheatEngine/autoassembler.pas, line 3265).
        constexpr size_t numTries = 10;
        for (size_t i = 0; i < numTries; i++)
        {
            void* freeRegionNearby = FindFreeBlockForRegion(currentAlloc->m_PreferredAddr, currentAlloc->m_SizeToAllocate);
            allocBase = VirtualAlloc(freeRegionNearby, currentAlloc->m_SizeToAllocate, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            if (allocBase)
            {
                break;
            }
        }
        if (!allocBase)
        {
            throw(std::exception("AssemblerContext::AllocateVariables(): Failed to allocate."));
        }
        currentAlloc->m_SuccessfulAllocBase = (uintptr_t)allocBase;
        currentAlloc->m_ResolvedAddr = currentAlloc->m_SuccessfulAllocBase;
    }
}
void AssemblerContext::ResolveSymbolAddresses()
{
    for (auto& currentLabel : m_LabelsQuickAccess)
    {
        currentLabel->m_ResolvedAddr = currentLabel->m_AssignedInWhichSymbol->m_ResolvedAddr.value() + currentLabel->m_Offset;
    }
}
void AssemblerContext::ResolveSymbolReferences()
{
    // The addresses of all symbols are already established, now I go through code blocks
    // that reference them, and update the `m_resultantCode` arrays with correct addresses (absolute or relative).
    for (auto& currentSymbol : m_Symbols)
    {
        for (SymbolMention& ref : currentSymbol->m_ReferencesToResolve)
        {
            uintptr_t writeAt = (uintptr_t)&ref.m_Symbol.m_resultantCode[ref.m_OffsetFromSymbolStart];
            uintptr_t whereTheReferenceWillBePlacedWhenActivated = ref.m_Symbol.m_ResolvedAddr.value() + ref.m_OffsetFromSymbolStart;
            int32_t writeWhat;
            if (SymbolMentionTypeRelative* relativeRef = std::get_if<SymbolMentionTypeRelative>(&ref.m_ReferenceOption))
            {
                // Relative reference. Calculate the correct offset between the referenced symbol and
                // the location where the reference will be written.
                uintptr_t calculateOffsetRelativeToWhere = whereTheReferenceWillBePlacedWhenActivated + relativeRef->m_HowManyBytesUntilTheEndOfOpcodeIncludingThese4bytes;
                writeWhat = (int32_t)(currentSymbol->m_ResolvedAddr.value() - calculateOffsetRelativeToWhere);
                *(int32_t*)writeAt = writeWhat;
            }
            else if (SymbolMentionTypeAbsolute* absoluteRef = std::get_if<SymbolMentionTypeAbsolute>(&ref.m_ReferenceOption))
            {
                switch (absoluteRef->m_AbsoluteAddressWidth)
                {
                case 4:
                {
                    uint32_t writeWhat = (uint32_t)currentSymbol->m_ResolvedAddr.value();
                    *(uint32_t*)writeAt = writeWhat;
                    break;
                }
                case 8:
                {
                    uint64_t writeWhat = (uint64_t)currentSymbol->m_ResolvedAddr.value();
                    *(uint64_t*)writeAt = writeWhat;
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

std::optional<uintptr_t> AutoAssemblerCodeHolder_Base::RETURN_TO_RIGHT_AFTER_STOLEN_BYTES;
AutoAssemblerCodeHolder_Base::AutoAssemblerCodeHolder_Base()
    : m_ctx(std::make_unique<AssemblerContext>())
{}
void AutoAssemblerCodeHolder_Base::PresetScript_CCodeInTheMiddle(uintptr_t whereToInject, size_t howManyBytesStolen, CCodeInTheMiddleFunctionPtr_t receiverFunc, std::optional<uintptr_t> whereToReturn, bool isNeedToExecuteStolenBytesAfterwards)
{
    std::stringstream ss;
    ss << std::hex << whereToInject;
    std::string symbolsBaseName = "injectAt_" + ss.str();
    DEFINE_ADDR_NAMED(injectAt, symbolsBaseName, whereToInject);
    uintptr_t injectionReturnAddr = whereToReturn ? whereToReturn.value() : whereToInject + howManyBytesStolen;
    DEFINE_ADDR_NAMED(injection_return, symbolsBaseName + "__return", injectionReturnAddr);

    ALLOC_NAMED(newmem, symbolsBaseName + "__newmem", 0x1000, whereToInject);
    LABEL_NAMED(ccode_flattened, symbolsBaseName + "__ccode_flattened");
    LABEL_NAMED(cave_entrance, symbolsBaseName + "__cave_entrance");

    injectAt = {
        0xE9, RIP(cave_entrance),
        nop(howManyBytesStolen - 5),
    };
    newmem = {
        PutLabel(ccode_flattened),
        "9C                  "              //  - pushfq
        "50                  "              //  - push rax
        "48 8B C4            "              //  - mov rax,rsp
        "48 83 E4 F0         "              //  - and rsp,-10               // align down to 0x10-byte boundary
        "48 81 EC A0020000   "              //  - sub rsp,000002A0
        "0FAE 44 24 20       "              //  - fxsave [rsp+20]
        "48 89 9C 24 20020000"              //  - mov [rsp+00000220],rbx
        "48 89 8C 24 28020000"              //  - mov [rsp+00000228],rcx
        "48 89 94 24 30020000"              //  - mov [rsp+00000230],rdx
        "48 89 B4 24 38020000"              //  - mov [rsp+00000238],rsi
        "48 89 BC 24 40020000"              //  - mov [rsp+00000240],rdi
        "48 89 84 24 48020000"              //  - mov [rsp+00000248],rax
        "48 89 AC 24 50020000"              //  - mov [rsp+00000250],rbp
        "4C 89 84 24 58020000"              //  - mov [rsp+00000258],r8
        "4C 89 8C 24 60020000"              //  - mov [rsp+00000260],r9
        "4C 89 94 24 68020000"              //  - mov [rsp+00000268],r10
        "4C 89 9C 24 70020000"              //  - mov [rsp+00000270],r11
        "4C 89 A4 24 78020000"              //  - mov [rsp+00000278],r12
        "4C 89 AC 24 80020000"              //  - mov [rsp+00000280],r13
        "4C 89 B4 24 88020000"              //  - mov [rsp+00000288],r14
        "4C 89 BC 24 90020000"              //  - mov [rsp+00000290],r15
        "48 8D 4C 24 20      "              //  - lea rcx,[rsp+20]
        "FF 15 80000000      "              //  - call qword ptr [label_externalFuncAddrBelow]  - doesn't need RIP-adjustment, because that label goes immediately after the end of current function, and distance to it is known.
        "4C 8B BC 24 90020000"              //  - mov r15,[rsp+00000290]
        "4C 8B B4 24 88020000"              //  - mov r14,[rsp+00000288]
        "4C 8B AC 24 80020000"              //  - mov r13,[rsp+00000280]
        "4C 8B A4 24 78020000"              //  - mov r12,[rsp+00000278]
        "4C 8B 9C 24 70020000"              //  - mov r11,[rsp+00000270]
        "4C 8B 94 24 68020000"              //  - mov r10,[rsp+00000268]
        "4C 8B 8C 24 60020000"              //  - mov r9,[rsp+00000260]
        "4C 8B 84 24 58020000"              //  - mov r8,[rsp+00000258]
        "48 8B AC 24 50020000"              //  - mov rbp,[rsp+00000250]
        "48 8B BC 24 40020000"              //  - mov rdi,[rsp+00000240]
        "48 8B B4 24 38020000"              //  - mov rsi,[rsp+00000238]
        "48 8B 94 24 30020000"              //  - mov rdx,[rsp+00000230]
        "48 8B 8C 24 28020000"              //  - mov rcx,[rsp+00000228]
        "48 8B 9C 24 20020000"              //  - mov rbx,[rsp+00000220]
        "0FAE 4C 24 20       "              //  - fxrstor [rsp+20]
        "48 8B A4 24 48020000"              //  - mov rsp,[rsp+00000248]
        "58                  "              //  - pop rax
        "9D                  "              //  - popfq
        "C3                  "              //  - ret
    //label_externalFuncAddrBelow:
        , dq((unsigned long long)receiverFunc),
        PutLabel(cave_entrance),
        "E8", RIP(ccode_flattened),         //  - call ccode_flattened
    };
    if (isNeedToExecuteStolenBytesAfterwards)
    {
        ByteVector&& currentBytes = injectAt.CopyCurrentBytes(howManyBytesStolen);
        newmem += {db(std::move(currentBytes.m_bytes))};
    }
    newmem += {
        0xE9, RIP(injection_return),        //  - jmp injection_return
    };
}
void AutoAssemblerCodeHolder_Base::PresetScript_NOP(uintptr_t whereToInject, size_t howManyBytesToNOP)
{
    std::stringstream ss;
    ss << std::hex << whereToInject;
    std::string symbolsBaseName = "injectAt_" + ss.str();
    DEFINE_ADDR_NAMED(injectAt, symbolsBaseName, whereToInject);
    injectAt = { nop(howManyBytesToNOP) };
}
