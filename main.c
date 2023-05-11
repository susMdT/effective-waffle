#include <windows.h>
#include <stdio.h>
#include <Psapi.h>
typedef NTSTATUS(NTAPI* TPALLOCWORK)(PTP_WORK* ptpWrk, PTP_WORK_CALLBACK pfnwkCallback, PVOID OptionalArg, PTP_CALLBACK_ENVIRON CallbackEnvironment);
typedef VOID(NTAPI* TPPOSTWORK)(PTP_WORK);
typedef VOID(NTAPI* TPRELEASEWORK)(PTP_WORK);
typedef VOID(NTAPI* TPWAITFORWORK)(PTP_WORK, BOOL);

typedef struct
{
    DWORD	Length;
    DWORD	MaximumLength;
    PVOID	Buffer;
} USTRING;

typedef struct _CALLBACK_ARGS {
    PVOID MessageBoxA;                   // 0x0
    PVOID text;                          // 0x8
    PVOID caption;                       // 0x10
    PVOID gadget_pop_rcx_ret;            // 0x18 address of a pop rcx;
    PVOID gadget_pop_rdx_ret;            // 0x20 address of a pop rdx;
    PVOID gadget_pop_r8_ret;             // 0x28 address of a pop r8;
    PVOID gadget_pop_r9_ret;             // 0x30 address of a pop r9;
    PVOID gadget_pop_rdx_rcx_r8_r9_ret;  // 0x38 address of a pop rdx; pop rcx; pop r8; pop r9; ret;
    PVOID pVirtualProtect;               // 0x40 address of VirtualProtect
    PVOID pSystemFunction032;            // 0x48 address of SystemFunction032
    PVOID pSleep;                        // 0x50 address of Sleep
    PVOID pImage;                        // 0x58 address of the USTRING containing encryption information for Sys032
    PVOID pKey;                          // 0x60 address of the key buffer
    LONG  SleepTime;                     // 0x68 sleep duration
    PVOID OldProtect;                    // 0x70 address of the oldprotect value for virtualprotect
    DWORD BytesToProtect;                // 0x78 amount of bytes to protect for virtualprotect
    PVOID pMemory;                       // 0x80 address of the bytes to protect for virtual protect
    PVOID gadget_add_rsp_28_ret;         // 0x88 address of a add rsp 28h; ret;
} CALLBACK_ARGS, *PCALLBACK_ARGS;

extern VOID CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
typedef NTSTATUS(WINAPI* NTCONTINUE)(PCONTEXT, BOOLEAN);
void* GetGadget(char* gadget) {
    // This function is written like shit (i am chatgpt abuser), please fix
    int counter = 0;
    char* moduleName = NULL;
    HMODULE ntdll = NULL;
    do {
        switch (counter)
        {
             case 0:
                 moduleName = "ntdll.dll";
                 break;
             case 1:
                 moduleName = "kernel32.dll";
                 break;
             case 2: // Server 2016 finds pop r8; ret here
                 moduleName = "shell32.dll";
                 LoadLibraryA(moduleName);
                 break;
             case 3:
                 moduleName = "kernelbase.dll";
                 break;

        }  
        counter++;
        if (counter > 3) {
            break;
        }
        ntdll = GetModuleHandleA(moduleName);
        if (ntdll == NULL) {
            printf("Could not load module\n");
            return NULL; // handle error
        }

        // get information about the ntdll module
        MODULEINFO mi;
        if (!GetModuleInformation(GetCurrentProcess(), ntdll, &mi, sizeof(mi))) {
            printf("getmoduleinformation error\n");
            return NULL; // handle error
        }

        // find the address of the "\x90" bytes in the .text section of the ntdll module
        IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)((char*)ntdll + ((IMAGE_DOS_HEADER*)ntdll)->e_lfanew); // address of the PE header
        IMAGE_SECTION_HEADER* section_header = IMAGE_FIRST_SECTION(nt_headers); // address of the first section header

        for (int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
            if (strcmp((char*)section_header->Name, ".text") == 0) { // if the section name is ".text"
                char* start_of_section = (char*)mi.lpBaseOfDll + section_header->VirtualAddress; // address of the start of the section
                char* end_of_section = start_of_section + section_header->Misc.VirtualSize; // address of the end of the section
                for (char* p = start_of_section; p < end_of_section; p++) {
                    if (memcmp(p, gadget, strlen(gadget)) == 0) {
                        printf("Found gadget at 0x%llx in %s\n", p, moduleName);
                        return p;
                    }
                }
            }
            section_header++; // move to the next section header
        }
    } while (1);
    printf("Could not find gadget\n");
    return NULL; 
}

void tpsleep(DWORD SleepTime) {
    char* text = "Hello";
    DWORD OldProtect = 0;

    USTRING Img = { 0 };
    PVOID ImageBase = GetModuleHandleA(NULL);
    Img.Buffer = ImageBase;
    DWORD ImageSize = ((PIMAGE_NT_HEADERS)((PBYTE)ImageBase + ((PIMAGE_DOS_HEADER)ImageBase)->e_lfanew))->OptionalHeader.SizeOfImage;
    Img.Length = Img.MaximumLength = ImageSize;

    USTRING Key = { 0 };
    CHAR    KeyBuf[16] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
    Key.Buffer = KeyBuf;
    Key.Length = Key.MaximumLength = 16;

    CALLBACK_ARGS c_args = { 0 };
    c_args.MessageBoxA = GetProcAddress(LoadLibraryA("user32.dll"), "MessageBoxA");
    c_args.text = text;
    c_args.caption = text;
    c_args.gadget_pop_rcx_ret = GetGadget("\x59\xc3");
    c_args.gadget_pop_rdx_ret = GetGadget("\x5a\xc3");
    c_args.gadget_pop_r8_ret = GetGadget("\x41\x58\xc3");
    c_args.gadget_pop_r9_ret = GetGadget("\x41\x59\xc3");
    c_args.gadget_pop_rdx_rcx_r8_r9_ret = GetGadget("\x5A\x59\x41\x58\x41\x59\xC3");
    c_args.pVirtualProtect = VirtualProtect;
    c_args.pSystemFunction032 = GetProcAddress(LoadLibraryA("Advapi32"), "SystemFunction032");
    c_args.pSleep = WaitForSingleObject;
    c_args.pImage = &Img;
    c_args.pKey = &Key;
    c_args.SleepTime = SleepTime;
    c_args.OldProtect = &OldProtect;
    c_args.BytesToProtect = Img.Length;
    c_args.pMemory = Img.Buffer;
    c_args.gadget_add_rsp_28_ret = GetGadget("\x48\x83\xc4\x28\xC3");

    //GetGadget("\x48\x83\xc4\x18\xC3"); // sub rsp 18h
    //GetGadget("\x59\x41\x58\x41\x59\xc3"); //pop rcx, r8, r9


    FARPROC pTpAllocWork = GetProcAddress(GetModuleHandleA("ntdll"), "TpAllocWork");
    FARPROC pTpPostWork = GetProcAddress(GetModuleHandleA("ntdll"), "TpPostWork");
    FARPROC pTpWaitForWork = GetProcAddress(GetModuleHandleA("ntdll"), "TpWaitForWork");
    FARPROC pTpReleaseWork = GetProcAddress(GetModuleHandleA("ntdll"), "TpReleaseWork");

    PTP_WORK WorkReturn = NULL;
    ((TPALLOCWORK)pTpAllocWork)(&WorkReturn, (PTP_WORK_CALLBACK)WorkCallback, &c_args, NULL);
    ((TPPOSTWORK)pTpPostWork)(WorkReturn);
    ((TPWAITFORWORK)pTpWaitForWork)(WorkReturn, FALSE);
    ((TPRELEASEWORK)pTpReleaseWork)(WorkReturn);
}
int main() {
    DWORD i = 0;
    while (1) {
        printf("On iteration %d\n", i);
        tpsleep(1000);
        i++;
    }
    
    return 0;
}
