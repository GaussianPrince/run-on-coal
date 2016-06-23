#include "stdafx.h"
#include "Scene/RenderTarget.h"

ROC::RenderTarget::RenderTarget()
{
    m_type = -1;
    m_bTexture = false;
    m_bRenderBuffer = false;
    m_bFrameBuffer = false;
}
ROC::RenderTarget::~RenderTarget()
{
    Clear();
}

void ROC::RenderTarget::Clear()
{
    m_type = -1;
    if(m_bTexture)
    {
        glDeleteTextures(1,&m_texture);
        m_bTexture = false;
    }
    if(m_bRenderBuffer)
    {
        glDeleteRenderbuffers(1,&m_renderBuffer);
        m_bRenderBuffer = false;
    }
    if(m_bFrameBuffer)
    {
        glDeleteFramebuffers(1,&m_frameBuffer);
        m_bFrameBuffer = false;
    }
}

bool ROC::RenderTarget::Create(unsigned int f_num,glm::ivec2 &f_size,unsigned int f_type)
{
    if(f_type > RenderTargetType::RGBF) return false;
    m_type = f_type;
    m_bRenderBuffer = false;

    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    m_bFrameBuffer = true;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    switch(m_type)
    {
        case RenderTargetType::Depth:
        {
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32F, f_size.x, f_size.y, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);  
        } break;
        case RenderTargetType::RGB:
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, f_size.x, f_size.y, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
            break;
        case RenderTargetType::RGBA:
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, f_size.x, f_size.y, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
            break;
        case RenderTargetType::RGBF:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, f_size.x, f_size.y,0, GL_RGB, GL_FLOAT, NULL);
            break;
    }
    m_bTexture = true;

    if(m_type > RenderTargetType::Depth)
    {
        glGenRenderbuffers(1, &m_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, f_size.x, f_size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);
        m_bRenderBuffer = true;

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+f_num, GL_TEXTURE_2D,m_texture, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0+f_num);
        glReadBuffer(GL_NONE);
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        m_error.append("Unable to create RT, framebuffer status ");
        m_error.append(std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
        Clear();
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    std::memcpy(&m_size,&f_size,sizeof(glm::ivec2));
    return true;
}

void ROC::RenderTarget::BindTexture(unsigned int f_bind)
{
    if(m_type == -1) return;
    if(f_bind) glActiveTexture(GL_TEXTURE0+f_bind);
    glBindTexture(GL_TEXTURE_2D,m_texture);
    if(f_bind) glActiveTexture(GL_TEXTURE0);
}

void ROC::RenderTarget::Enable()
{
    if(m_type == -1) return;
    glBindFramebuffer(GL_FRAMEBUFFER,m_frameBuffer);
}
void ROC::RenderTarget::GetSize(glm::ivec2 &f_size)
{
    std::memcpy(&f_size,&m_size,sizeof(glm::ivec2));
}

bool ROC::RenderTarget::IsColored()
{
    return (m_type >= RenderTargetType::RGB && m_type <= RenderTargetType::RGBA);
}
bool ROC::RenderTarget::IsTransparent()
{
    return (m_type == RenderTargetType::RGBA);
}
bool ROC::RenderTarget::IsDepthable()
{
    return (m_type == RenderTargetType::Depth);
}

GLuint ROC::RenderTarget::GetTexture()
{
    return m_texture;
}

void ROC::RenderTarget::GetError(std::string &f_str)
{
    f_str.append(m_error);
}