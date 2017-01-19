#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/ElementManager.h"
#include "Managers/LuaManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/LogManager.h"
#include "Lua/ArgReader.h"
#include "Lua/LuaDefinitions.Utils.h"
#include "Utils/Utils.h"

namespace ROC
{
namespace Lua
{

const std::vector<std::string> g_elementTypeName
{
    "geometry", "model", "animation", "scene", "camera", "light",
    "rendertarget", "shader", "sound", "texture", "font", "file", "collision",
};

int disabledFunction(lua_State *f_vm)
{
    lua_pushboolean(f_vm, 0);
    return 1;
}

int logPrint(lua_State *f_vm)
{
    std::string l_text;
    ArgReader argStream(f_vm, LuaManager::m_corePointer);
    argStream.ReadText(l_text);
    if(!argStream.HasErrors())
    {
        LuaManager::m_corePointer->GetLogManager()->Log(l_text);
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int isElement(lua_State *f_vm)
{
    void *l_pointer = NULL;
    ArgReader argStream(f_vm, LuaManager::m_corePointer);
    argStream.ReadPointer(&l_pointer);
    argStream.PushBoolean(!argStream.HasErrors() ? LuaManager::m_corePointer->GetMemoryManager()->IsValidMemoryPointer(l_pointer) : false);
    return argStream.GetReturnValue();
}

int getElementType(lua_State *f_vm)
{
    void *l_pointer = NULL;
    ArgReader argStream(f_vm, LuaManager::m_corePointer);
    argStream.ReadPointer(&l_pointer);
    if(!argStream.HasErrors())
    {
        int l_type = LuaManager::m_corePointer->GetMemoryManager()->GetMemoryPointerType(l_pointer);
        argStream.PushText((l_type != -1) ? g_elementTypeName[l_type] : "invalid");
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int getTickCount(lua_State *f_vm)
{
    ArgReader argStream(f_vm, LuaManager::m_corePointer);
    lua_Integer l_tick = 0;
    l_tick = static_cast<lua_Integer>(GetTickCount());
    argStream.PushInteger(l_tick);
    return argStream.GetReturnValue();
}

}
}