#pragma once
#include "Interfaces/IShader.h"
#include "Elements/Element.h"

namespace ROC
{

class Drawable;
class Light;
class Pool;
class ShaderUniform;

class Shader final : public Element, public virtual IShader
{
    GLuint m_program;
    bool m_active;

    enum ShaderDefaultUniform : size_t
    {
        SDU_Projection,
        SDU_View,
        SDU_ViewProjection,
        SDU_Model,
        SDU_BoneMatrices,
        SDU_CameraPosition,
        SDU_CameraDirection,
        SDU_LightData,
        SDU_LightsCount,
        SDU_MaterialParam,
        SDU_MaterialType,
        SDU_Animated,
        SDU_DiffuseTexture,
        SDU_Time,
        SDU_Color,

        SDU_UniformCount
    };
    ShaderUniform *m_defaultUniforms[SDU_UniformCount];

    std::unordered_map<std::string, ShaderUniform*> m_uniformMap;
    std::unordered_map<std::string, ShaderUniform*>::iterator m_uniformMapEnd;

    Pool *m_bindPool;
    struct DrawableBindData
    {
        Drawable *m_element;
        int m_slot;
        ShaderUniform *m_uniform;
    };
    std::vector<DrawableBindData> m_drawableBind;
    static int ms_drawableMaxCount;

    std::string m_error;

    const static std::string ms_defaultDefines;

    Shader(const Shader &that) = delete;
    Shader& operator=(const Shader &that) = delete;

    void SetupUniformsAndLocations();
    void FindDefaultUniform(ShaderDefaultUniform f_sud, const char *f_name, unsigned int f_type);
public:
    ShaderUniform* GetUniform(const std::string &f_uniform);

    bool Attach(Drawable *f_drawable, const std::string &f_uniform);
    bool Detach(Drawable *f_drawable);
    bool HasAttached(Drawable *f_drawable) const;
protected:
    Shader();
    ~Shader();

    bool Load(const std::string &f_vpath, const std::string &f_fpath, const std::string &f_gpath);
    inline const std::string& GetError() const { return m_error; }

    void SetProjectionMatrix(const glm::mat4 &f_value);
    void SetViewMatrix(const glm::mat4 &f_value);
    void SetViewProjectionMatrix(const glm::mat4 &f_value);
    void SetModelMatrix(const glm::mat4 &f_value);
    void SetBoneMatrices(const std::vector<glm::mat4> &f_value);
    void SetCameraPosition(const glm::vec3 &f_value);
    void SetCameraDirection(const glm::vec3 &f_value);
    void SetLightsData(const std::vector<Light*> &f_data);
    void SetMaterialParam(const glm::vec4 &f_value);
    void SetMaterialType(int f_value);
    void SetAnimated(bool f_value);
    void SetTime(float f_value);
    void SetColor(const glm::vec4 &f_value);

    void Enable();
    void Disable();
    inline bool IsActive() const { return m_active; }

    static void UpdateDrawableMaxCount();

    void OnParentLinkDestroyed(Element *f_parent);
    void OnChildLinkDestroyed(Element *f_child);

    friend class ElementManager;
    friend class RenderManager;
    friend class SfmlManager;
    friend class Scene;
private:
    IShaderUniform* GetIUniform(const std::string &f_uniform);
    bool Attach(IDrawable *f_drawable, const std::string &f_uniform);
    bool Detach(IDrawable *f_drawable);
    bool HasAttached(IDrawable *f_drawable) const;
};

}
