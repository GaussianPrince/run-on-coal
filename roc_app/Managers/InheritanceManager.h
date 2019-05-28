#pragma once
#include "Interfaces/IInheritanceManager.h"

namespace ROC
{

class Core;
class Element;
class Drawable;
class Animation;
class Collision;
class Camera;
class Geometry;
class Light;
class Model;
class RenderTarget;
class Scene;
class Shader;

class InheritanceManager final : public IInheritanceManager
{
    Core *m_core;

    std::multimap<Element*, Element*> m_inheritMap; // Child <-> Parent

    void AddInheritance(Element *f_child, Element *f_parent);
    void RemoveInheritance(Element *f_child, Element *f_parent);

    static void InheritanceBreak(Element *f_child, Element *f_parent);

    InheritanceManager(const InheritanceManager &that) = delete;
    InheritanceManager& operator=(const InheritanceManager &that) = delete;

    // Interfaces reroute
    bool AttachModelToModel(IModel *f_model, IModel *f_parent, int f_bone = -1);
    bool DetachModel(IModel *f_model);
    bool SetModelAnimation(IModel *f_model, IAnimation *f_anim);
    bool RemoveModelAnimation(IModel *f_model);
    bool SetModelCollision(IModel *f_model, ICollision *f_col);
    bool RemoveModelCollision(IModel *f_model);
    bool AddModelToScene(IScene *f_scene, IModel *f_model);
    bool RemoveModelFromScene(IScene *f_scene, IModel *f_model);
    bool SetSceneCamera(IScene *f_scene, ICamera *f_camera);
    bool RemoveSceneCamera(IScene *f_scene);
    bool AddSceneLight(IScene *f_scene, ILight *f_light);
    bool RemoveSceneLight(IScene *f_scene, ILight *f_light);
    bool SetSceneRenderTarget(IScene *f_scene, IRenderTarget*f_rt);
    bool RemoveSceneRenderTarget(IScene *f_scene);
    bool SetSceneShader(IScene *f_scene, IShader *f_shader);
    bool RemoveSceneShader(IScene *f_scene);
    bool AttachDrawableToShader(IShader *f_shader, IDrawable *f_drawable, const std::string &f_uniform);
    bool DetachDrawableFromShader(IShader *f_shader, IDrawable *f_drawable);
public:
    bool AttachModelToModel(Model *f_model, Model *f_parent, int f_bone = -1);
    bool DetachModel(Model *f_model);
    bool SetModelAnimation(Model *f_model, Animation *f_anim);
    bool RemoveModelAnimation(Model *f_model);
    bool SetModelCollision(Model *f_model, Collision *f_col);
    bool RemoveModelCollision(Model *f_model);

    bool AddModelToScene(Scene *f_scene, Model *f_model);
    bool RemoveModelFromScene(Scene *f_scene, Model *f_model);
    bool SetSceneCamera(Scene *f_scene, Camera *f_camera);
    bool RemoveSceneCamera(Scene *f_scene);
    bool AddSceneLight(Scene *f_scene, Light *f_light);
    bool RemoveSceneLight(Scene *f_scene, Light *f_light);
    bool SetSceneRenderTarget(Scene *f_scene, RenderTarget *f_rt);
    bool RemoveSceneRenderTarget(Scene *f_scene);
    bool SetSceneShader(Scene *f_scene, Shader *f_shader);
    bool RemoveSceneShader(Scene *f_scene);

    bool AttachDrawableToShader(Shader *f_shader, Drawable *f_drawable, const std::string &f_uniform);
    bool DetachDrawableFromShader(Shader *f_shader, Drawable *f_drawable);
protected:
    explicit InheritanceManager(Core *f_core);
    ~InheritanceManager();

    void RemoveChildRelations(Element *f_child);
    void RemoveParentRelations(Element *f_parent);

    void SetModelGeometry(Model *f_model, Geometry *f_geometry);

    friend class Core;
    friend class ElementManager;
};

}