#include "stdafx.h"

#include "Lua/LuaDefs/LuaFontDef.h"

#include "Module/LuaModule.h"
#include "Lua/ArgReader.h"
#include "Lua/LuaDefs/LuaElementDef.h"
#include "Utils/EnumUtils.h"
#include "Utils/LuaUtils.h"

extern const std::vector<std::string> g_FilteringTypesTable; // defined in LuaDrawableDef

void LuaFontDef::Init(lua_State *f_vm)
{
    LuaUtils::AddClass(f_vm, "Font", Create);
    LuaUtils::AddClassMethod(f_vm, "draw", Draw);
    LuaElementDef::AddHierarchyMethods(f_vm);
    LuaUtils::AddClassFinish(f_vm);
}

int LuaFontDef::Create(lua_State *f_vm)
{
    // element Font(str path, int size [, int atlasX = 256, int atlasY = 256, str filtering = "nearest"])
    std::string l_path;
    int l_size;
    glm::ivec2 l_atlasSize(256);
    std::string l_filter;
    ArgReader argStream(f_vm);
    argStream.ReadText(l_path);
    argStream.ReadInteger(l_size);
    argStream.ReadNextInteger(l_atlasSize.x);
    argStream.ReadNextInteger(l_atlasSize.y);
    argStream.ReadNextText(l_filter);
    if(!argStream.HasErrors() && !l_path.empty() && l_size > 0)
    {
        int l_filteringType = EnumUtils::ReadEnumVector(l_filter, g_FilteringTypesTable);
        ROC::IFont *l_font = LuaModule::GetModule()->GetEngineCore()->GetElementManager()->CreateFont_(l_path, l_size, l_atlasSize, l_filteringType);
        l_font ? argStream.PushElement(l_font) : argStream.PushBoolean(false);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int LuaFontDef::Draw(lua_State *f_vm)
{
    // bool Font:draw(float x, float y, str text [, float colorR = 1, float colorG = 1, float colorB = 1, float colorA = 1])
    ROC::IFont *l_font;
    glm::vec2 l_pos;
    std::string l_text;
    glm::vec4 l_color(1.f);
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_font);
    for(int i = 0; i < 2; i++) argStream.ReadNumber(l_pos[i]);
    argStream.ReadText(l_text);
    for(int i = 0; i < 4; i++) argStream.ReadNextNumber(l_color[i]);
    if(!argStream.HasErrors() && !l_text.empty())
    {
        bool l_result = LuaModule::GetModule()->GetEngineCore()->GetRenderManager()->Render(l_font, l_pos, l_text, l_color);
        argStream.PushBoolean(l_result);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}