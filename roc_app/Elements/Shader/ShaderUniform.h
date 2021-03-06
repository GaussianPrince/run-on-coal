#pragma once
#include "Interfaces/IShaderUniform.h"

namespace ROC
{

class ShaderUniform final : public IShaderUniform
{
    unsigned int m_type;
    int m_uniform;
    union
    {
        float m_float[16];
        int m_int[4];
        unsigned int m_bool[4];
    };
    bool m_active;
    bool m_updated;

    ShaderUniform(const ShaderUniform &that) = delete;
    ShaderUniform& operator=(const ShaderUniform &that) = delete;
public:
    enum ShaderUniformType
    {
        SUT_Float = GL_FLOAT,
        SUT_Float2 = GL_FLOAT_VEC2,
        SUT_Float3 = GL_FLOAT_VEC3,
        SUT_Float4 = GL_FLOAT_VEC4,
        SUT_Int = GL_INT,
        SUT_Int2 = GL_INT_VEC2,
        SUT_Int3 = GL_INT_VEC3,
        SUT_Int4 = GL_INT_VEC4,
        SUT_Bool = GL_BOOL,
        SUT_Bool2 = GL_BOOL_VEC2,
        SUT_Bool3 = GL_BOOL_VEC3,
        SUT_Bool4 = GL_BOOL_VEC4,
        SUT_Mat2 = GL_FLOAT_MAT2,
        SUT_Mat3 = GL_FLOAT_MAT3,
        SUT_Mat4 = GL_FLOAT_MAT4,
        SUT_Sampler = GL_SAMPLER_2D,
        SUT_ShadowSampler = GL_SAMPLER_2D_SHADOW,
        SUT_CubeSampler = GL_SAMPLER_CUBE
    };

    unsigned int GetType() const;
    bool IsSampler2D() const;
    bool IsSamplerCube() const;

    bool SetValue(float f_val);
    bool SetValue(const glm::vec2 &f_val);
    bool SetValue(const glm::vec3 &f_val);
    bool SetValue(const glm::vec4 &f_val);

    bool SetValue(int f_val);
    bool SetValue(const glm::ivec2 &f_val);
    bool SetValue(const glm::ivec3 &f_val);
    bool SetValue(const glm::ivec4 &f_val);

    bool SetValue(bool f_val);
    bool SetValue(const glm::bvec2 &f_val);
    bool SetValue(const glm::bvec3 &f_val);
    bool SetValue(const glm::bvec4 &f_val);

    bool SetValue(const glm::mat2 &f_val);
    bool SetValue(const glm::mat3 &f_val);
    bool SetValue(const glm::mat4 &f_val);

    bool IsActive() const;
protected:
    ShaderUniform(unsigned int f_type, int f_uniform);
    ~ShaderUniform();

    inline void SetActive(bool f_active) { m_active = f_active; }

    bool SetSampler(int f_sampler);
    inline int GetUniform() const { return m_uniform; }

    void Update();

    friend class Shader;
};

}
