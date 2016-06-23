#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/ElementManager.h"
#include "Managers/InheritanceManager.h"
#include "Managers/LogManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/PreRenderManager.h"
#include "Managers/RenderManager.h"
#include "Model/Animation.h"
#include "Model/Geometry.h"
#include "Model/Model.h"
#include "Scene/Collision.h"
#include "Scene/Cursor.h"
#include "Scene/File.h"
#include "Scene/Font.h"
#include "Scene/RenderTarget.h"
#include "Scene/Scene.h"
#include "Scene/Shader.h"
#include "Scene/Sound.h"
#include "Scene/Texture.h"
#include "Utils/Utils.h"

ROC::ElementManager::ElementManager(Core *f_core)
{
    m_core = f_core;
    m_locked = false;
}
ROC::ElementManager::~ElementManager()
{
}

void ROC::ElementManager::SetLock(bool f_lock)
{
    m_locked = f_lock;
}

ROC::Scene* ROC::ElementManager::CreateScene()
{
    ROC::Scene *l_scene= new Scene();
    if(!l_scene) return NULL;
    m_core->GetMemoryManager()->AddMemoryPointer(l_scene,ElementType::SceneElement);
    return l_scene;
}
bool ROC::ElementManager::DestroyScene(Scene *f_scene)
{
    m_core->GetRenderManager()->CheckAndRemoveSceneIfCurrent(f_scene);
    m_core->GetInheritManager()->RemoveParentRelation(f_scene);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_scene,ElementType::SceneElement);
    delete f_scene;
    return true;
}

ROC::Camera* ROC::ElementManager::CreateCamera()
{
    Camera *l_camera = new Camera();
    if(!l_camera) return NULL;
    m_core->GetMemoryManager()->AddMemoryPointer(l_camera,ElementType::CameraElement);
    return l_camera;
}
bool ROC::ElementManager::DestroyCamera(Camera *f_cam)
{
    m_core->GetInheritManager()->RemoveChildRelation(f_cam);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_cam,ElementType::CameraElement);
    delete f_cam;
    return true;
}

ROC::Light* ROC::ElementManager::CreateLight()
{
    Light *l_light = new Light();
    if(!l_light) return NULL;
    m_core->GetMemoryManager()->AddMemoryPointer(l_light,ElementType::LightElement);
    return l_light;
}
bool ROC::ElementManager::DestroyLight(Light *f_light)
{
    m_core->GetInheritManager()->RemoveChildRelation(f_light);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_light,ElementType::LightElement);
    delete f_light;
    return true;
}

ROC::Animation* ROC::ElementManager::CreateAnimation(std::string &f_path)
{
    Animation *l_anim = new Animation();
    if(!l_anim) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(!l_anim->Load(l_path))
    {
        delete l_anim;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_anim,ElementType::AnimationElement);
    return l_anim;
}
bool ROC::ElementManager::DestroyAnimation(Animation *f_anim)
{
    m_core->GetInheritManager()->RemoveParentRelation(f_anim);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_anim,ElementType::AnimationElement);
    delete f_anim;
    return true;
}

ROC::Geometry* ROC::ElementManager::CreateGeometry(std::string &f_path, bool l_comp)
{
    Geometry *l_geometry = new Geometry();
    if(!l_geometry) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(!l_geometry->Load(l_path,l_comp))
    {
        delete l_geometry;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_geometry,ElementType::GeometryElement);
    return l_geometry;
}
bool ROC::ElementManager::DestroyGeometry(Geometry *f_geometry)
{
    m_core->GetInheritManager()->RemoveParentRelation(f_geometry);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_geometry,ElementType::GeometryElement);
    delete f_geometry;
    return true;
}

ROC::Model* ROC::ElementManager::CreateModel(Geometry *f_geometry)
{
    Model *l_model = new Model(f_geometry);
    if(!l_model) return NULL;
    m_core->GetInheritManager()->SetModelGeometry(l_model,f_geometry);
    m_core->GetMemoryManager()->AddMemoryPointer(l_model,ElementType::ModelElement);
    m_core->GetPreRenderManager()->AddModel(l_model);
    if(l_model->HasRigidSkeleton()) m_core->GetPhysicsManager()->AddRigidSkeleton(l_model);
    return l_model;
}
bool ROC::ElementManager::DestroyModel(Model *f_model)
{
    m_core->GetInheritManager()->RemoveParentRelation(f_model);
    m_core->GetInheritManager()->RemoveChildRelation(f_model);
    m_core->GetPreRenderManager()->RemoveModel(f_model);
    if(f_model->IsRigid()) m_core->GetPhysicsManager()->RemoveModelRigidity(f_model);
    if(f_model->HasRigidSkeleton()) m_core->GetPhysicsManager()->RemoveRigidSkeleton(f_model);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_model,ElementType::ModelElement);
    delete f_model;
    return true;
}

ROC::Shader* ROC::ElementManager::CreateShader(std::string &f_vpath, std::string &f_fpath, std::string &f_gpath)
{
    Shader *l_shader = new Shader();
    if(!l_shader) return NULL;
    std::string l_path[3],l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    if(f_vpath.length()) Utils::PathsJoin(l_wDir,f_vpath,l_path[0]);
    if(f_fpath.length()) Utils::PathsJoin(l_wDir,f_fpath,l_path[1]);
    if(f_gpath.length()) Utils::PathsJoin(l_wDir,f_gpath,l_path[2]);
    if(!l_shader->Load(l_path[0],l_path[1],l_path[2]))
    {
        std::string l_shaderError;
        l_shader->GetError(l_shaderError);
        if(l_shaderError.length())
        {
            std::string l_error("[");
            l_error.append(f_vpath);
            l_error.append(",");
            l_error.append(f_fpath);
            l_error.append(",");
            l_error.append(f_gpath);
            l_error.append("] -> ");
            l_error.append(l_shaderError);
            m_core->GetLogManager()->Log(l_error);
        }
        delete l_shader;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_shader,ElementType::ShaderElement);
    if(m_locked) m_core->GetRenderManager()->RestoreShader(NULL);
    return l_shader;
}
bool ROC::ElementManager::DestroyShader(Shader *f_shader)
{
    m_core->GetRenderManager()->CheckShaderForCurrent(f_shader);
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_shader,ElementType::ShaderElement);
    delete f_shader;
    return true;
}

ROC::Sound* ROC::ElementManager::CreateSound(std::string &f_path, bool f_loop)
{
    Sound *l_sound = new Sound(f_loop);
    if(!l_sound) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(!l_sound->Load(l_path))
    {
        delete l_sound;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_sound,ElementType::SoundElement);
    return l_sound;
}
bool ROC::ElementManager::DestroySound(Sound *f_sound)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_sound,ElementType::SoundElement);
    delete f_sound;
    return true;
}

ROC::RenderTarget* ROC::ElementManager::CreateRenderTarget(unsigned int f_num,glm::ivec2 &f_size,unsigned int f_type)
{
    RenderTarget *l_rt = new RenderTarget();
    if(!l_rt) return NULL;
    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(!l_rt->Create(f_num,f_size,f_type))
    {
        std::string l_error;
        l_rt->GetError(l_error);
        m_core->GetLogManager()->Log(l_error);
        delete l_rt;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_rt,ElementType::RenderTargetElement);
    return l_rt;
}
bool ROC::ElementManager::DestroyRenderTarget(RenderTarget *f_rt)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_rt,ElementType::RenderTargetElement);
    delete f_rt;
    return true;
}

ROC::Texture* ROC::ElementManager::CreateTexture(std::string &f_path, unsigned int f_type, bool f_compress)
{
    Texture *l_tex = new Texture();
    if(!l_tex) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(!l_tex->Load(l_path,f_type,f_compress))
    {
        delete l_tex;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_tex,ElementType::TextureElement);
    return l_tex;
}
ROC::Texture* ROC::ElementManager::CreateTexture(std::vector<std::string> &f_path, bool f_compress)
{
    Texture *l_tex = new Texture();
    if(!l_tex) return NULL;
    std::vector<std::string> l_vPath;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    for(auto iter : f_path)
    {
        Utils::PathsJoin(l_wDir,iter,l_path);
        l_vPath.push_back(l_path);
        l_path.clear();
    }
    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(!l_tex->LoadCubemap(l_vPath,f_compress))
    {
        delete l_tex;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_tex,ElementType::TextureElement);
    return l_tex;
}
bool ROC::ElementManager::DestroyTexture(Texture *f_tex)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_tex,ElementType::TextureElement);
    delete f_tex;
    return true;
}

ROC::Font* ROC::ElementManager::CreateFont_(std::string &f_path, int f_size)
{
    Font *l_font = new Font();
    if(!l_font) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(!l_font->LoadTTF(l_path,f_size))
    {
        delete l_font;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_font,ElementType::FontElement);
    return l_font;
}
bool ROC::ElementManager::DestroyFont(Font *f_font)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_font,ElementType::FontElement);
    delete f_font;
    return true;
}

ROC::Cursor* ROC::ElementManager::CreateCursor(std::string &f_path, bool f_sys)
{
    Cursor *l_cursor = new Cursor();
    if(!l_cursor) return NULL;
    std::string l_path;
    if(!f_sys)
    {
        std::string l_wDir;
        m_core->GetWorkingDirectory(l_wDir);
        Utils::PathsJoin(l_wDir,f_path,l_path);
    }
    else l_path.append(f_path);
    if(!l_cursor->Load(l_path,f_sys))
    {
        delete l_cursor;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_cursor,ElementType::CursorElement);
    return l_cursor;
}
bool ROC::ElementManager::DestroyCursor(Cursor *f_cursor)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_cursor,ElementType::CursorElement);
    delete f_cursor;
    return true;
}

ROC::File* ROC::ElementManager::CreateFile_(std::string &f_path)
{
    File *l_file = new File();
    if(!l_file) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(!l_file->Create(l_path,f_path))
    {
        delete l_file;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_file,ElementType::FileElement);
    return l_file;
}
ROC::File* ROC::ElementManager::OpenFile(std::string &f_path,bool f_ro)
{
    File *l_file = new File();
    if(!l_file) return NULL;
    std::string l_path,l_wDir;
    m_core->GetWorkingDirectory(l_wDir);
    Utils::PathsJoin(l_wDir,f_path,l_path);
    if(!l_file->Open(l_path,f_path,f_ro))
    {
        delete l_file;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_file,ElementType::FileElement);
    return l_file;
}
bool ROC::ElementManager::DestroyFile(File *f_file)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_file,ElementType::FileElement);
    delete f_file;
    return true;
}

ROC::Collision* ROC::ElementManager::CreateCollision(unsigned char f_type, glm::vec3 &f_size)
{
    Collision *l_col = new Collision();
    if(!l_col) return NULL;
    if(!l_col->Create(f_type,f_size))
    {
        delete l_col;
        return NULL;
    }
    m_core->GetMemoryManager()->AddMemoryPointer(l_col,ElementType::CollisionElement);
    m_core->GetPhysicsManager()->AddCollision(l_col);
    return l_col;
}
bool ROC::ElementManager::DestroyCollision(Collision *f_col)
{
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_col,ElementType::CollisionElement);
    m_core->GetPhysicsManager()->RemoveCollision(f_col);
    delete f_col;
    return true;
}

void ROC::ElementManager::DestroyByPointer(void *f_pointer,unsigned char f_type)
{
    //Called only at the end of work
    switch(f_type)
    {
        case ElementType::SceneElement:
            delete ((Scene*)f_pointer);
            break;
        case ElementType::CameraElement:
            delete ((Camera*)f_pointer);
            break;
        case ElementType::LightElement:
            delete ((Light*)f_pointer);
            break;
        case ElementType::ShaderElement:
            delete ((Shader*)f_pointer);
            break;
        case ElementType::GeometryElement:
            delete ((Geometry*)f_pointer);
            break;
        case ElementType::ModelElement:
            delete ((Model*)f_pointer);
            break;
        case ElementType::AnimationElement:
            delete ((Animation*)f_pointer);
            break;
        case ElementType::SoundElement:
            delete ((Sound*)f_pointer);
            break;
        case ElementType::RenderTargetElement:
            delete ((RenderTarget*)f_pointer);
            break;
        case ElementType::TextureElement:
            delete ((Texture*)f_pointer);
            break;
        case ElementType::FontElement:
            delete ((Font*)f_pointer);
            break;
        case ElementType::CursorElement:
            delete ((Cursor*)f_pointer);
            break;
        case ElementType::FileElement:
            delete ((File*)f_pointer);
            break;
        case ElementType::CollisionElement:
            delete ((Collision*)f_pointer);
            break;
    }
}