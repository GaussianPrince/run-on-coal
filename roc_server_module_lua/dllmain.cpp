#include "stdafx.h"

#include "Module/LuaModule.h"

LuaModule *g_Module = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    BOOL l_result = FALSE;
    switch(ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            l_result = TRUE;
            break;
        case DLL_PROCESS_DETACH:
        {
            if(g_Module)
            {
                LuaModule::Terminate();
                g_Module = nullptr;
            }
            l_result = TRUE;
        } break;
    }
    return l_result;
}

extern "C" __declspec(dllexport) ROC::IModule* ServerModuleInit(ROC::ICore *f_core)
{
    if(!g_Module)
    {
        if(LuaModule::Init(f_core)) g_Module = LuaModule::GetModule();
    }
    return g_Module;
}
