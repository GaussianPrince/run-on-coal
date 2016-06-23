#pragma once

namespace ROC 
{
namespace Lua 
{

int modelCreate(lua_State *f_vm);
int modelDestroy(lua_State *f_vm);
int modelGetType(lua_State *f_vm);
int modelSetPosition(lua_State *f_vm);
int modelGetPosition(lua_State *f_vm);
int modelSetRotation(lua_State *f_vm);
int modelGetRotation(lua_State *f_vm);
int modelSetScale(lua_State *f_vm);
int modelGetScale(lua_State *f_vm);
int modelSetMatrixUpdate(lua_State *f_vm);
int modelDraw(lua_State *f_vm);
int modelAttach(lua_State *f_vm);
int modelDettach(lua_State *f_vm);
int modelSetAnimation(lua_State *f_vm);
int modelPlayAnimation(lua_State *f_vm);
int modelPauseAnimation(lua_State *f_vm);
int modelResetAnimation(lua_State *f_vm);
int modelSetAnimationSpeed(lua_State *f_vm);
int modelGetAnimationSpeed(lua_State *f_vm);
int modelSetAnimationProgress(lua_State *f_vm);
int modelGetAnimationProgress(lua_State *f_vm);

int modelSetBonePosition(lua_State *f_vm);
int modelSetBoneRotation(lua_State *f_vm);
int modelSetBoneScale(lua_State *f_vm);

int modelSetRigidity(lua_State *f_vm);
int modelRemoveRigidity(lua_State *f_vm);
int modelGetMass(lua_State *f_vm);
int modelSetFriction(lua_State *f_vm);
int modelGetFriction(lua_State *f_vm);
int modelSetVelocity(lua_State *f_vm);
int modelGetVelocity(lua_State *f_vm);
int modelSetAngularVelocity(lua_State *f_vm);
int modelGetAngularVelocity(lua_State *f_vm);

}
}