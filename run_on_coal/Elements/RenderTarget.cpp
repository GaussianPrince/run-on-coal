#include "stdafx.h"

#include "Elements/RenderTarget.h"

ROC::RenderTarget::RenderTarget()
{
    m_elementType = ElementType::RenderTargetElement;
    m_elementTypeName.assign("RenderTarget");

    m_type = ROC_RENDERTARGET_TYPE_NONE;
    m_filtering = ROC_RENDERTARGET_FILTER_NONE;
    m_frameBuffer = 0U;
    m_renderBuffer = 0U;
    m_texture = 0U;
}
ROC::RenderTarget::~RenderTarget()
{
    Clear();
}

bool ROC::RenderTarget::Create(unsigned int f_num, const glm::ivec2 &f_size, int f_type, int f_filter)
{
    if(m_type == ROC_RENDERTARGET_TYPE_NONE)
    {
        m_type = f_type;
        btClamp(m_type,ROC_RENDERTARGET_TYPE_SHADOW, ROC_RENDERTARGET_TYPE_RGBAF);
        m_filtering = f_filter;
        btClamp(m_filtering, ROC_RENDERTARGET_FILTER_NEAREST, ROC_RENDERTARGET_FILTER_LINEAR);

        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        switch(m_type)
        {
            case ROC_RENDERTARGET_TYPE_SHADOW:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, f_size.x, f_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            } break;
            case ROC_RENDERTARGET_TYPE_RGB:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, f_size.x, f_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
                break;
            case ROC_RENDERTARGET_TYPE_RGBA:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_size.x, f_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                break;
            case ROC_RENDERTARGET_TYPE_RGBF:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, f_size.x, f_size.y, 0, GL_RGB, GL_FLOAT, NULL);
                break;
            case ROC_RENDERTARGET_TYPE_RGBAF:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, f_size.x, f_size.y, 0, GL_RGBA, GL_FLOAT, NULL);
                break;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST+m_filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST+m_filtering);

        if(m_type > ROC_RENDERTARGET_TYPE_SHADOW)
        {
            glGenRenderbuffers(1, &m_renderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, f_size.x, f_size.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + f_num, GL_TEXTURE_2D, m_texture, 0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + f_num);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
            glDrawBuffer(GL_NONE);
        }
        glReadBuffer(GL_NONE);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            m_error.assign("Unable to create RT, framebuffer status ");
            m_error.append(std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
            Clear();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            std::memcpy(&m_size, &f_size, sizeof(glm::ivec2));
        }
    }
    return (m_type != ROC_RENDERTARGET_TYPE_NONE);
}

void ROC::RenderTarget::Bind()
{
    if(m_type != ROC_RENDERTARGET_TYPE_NONE) glBindTexture(GL_TEXTURE_2D, m_texture);
}
void ROC::RenderTarget::Enable()
{
    if(m_type != ROC_RENDERTARGET_TYPE_NONE) glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

void ROC::RenderTarget::Clear()
{
    m_type = ROC_RENDERTARGET_TYPE_NONE;
    m_filtering = ROC_RENDERTARGET_FILTER_NONE;
    if(m_texture != 0U)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0U;
    }
    if(m_renderBuffer != 0U)
    {
        glDeleteRenderbuffers(1, &m_renderBuffer);
        m_renderBuffer = 0U;
    }
    if(m_frameBuffer != 0U)
    {
        glDeleteFramebuffers(1, &m_frameBuffer);
        m_frameBuffer = 0U;
    }
}
