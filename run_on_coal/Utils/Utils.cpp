#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/LogManager.h"
#include "Utils/Utils.h"

namespace ROC 
{
namespace Utils 
{

bool ReadFile(std::string &path, std::string &f_cont)
{
    std::ifstream l_file;
    l_file.open(path,std::ios::in);
    if(l_file.fail()) return false;

    std::ostringstream l_data;
    l_data << l_file.rdbuf();
    l_file.close();
    f_cont.append(l_data.str());
    return true;
}

GLint CheckShader(GLuint f_shader, std::string &f_path)
{
    GLint l_params = -1;
    glGetShaderiv (f_shader, GL_COMPILE_STATUS, &l_params);
    return l_params;
}

void GetScreenPosition(glm::vec3 &pos, glm::mat4 &proj, glm::mat4 &view, glm::ivec2 &window, glm::ivec2 &out)
{
    glm::vec4 l_result = proj*view*glm::vec4(pos,1.0f);
    l_result /= l_result.w;
    out = glm::ivec2(window.x/2*(1.f+l_result.x),window.y/2*(1.f+l_result.y));
}

int ReadEnumString(std::string &f_val, std::string f_enum)
{
    size_t first = f_enum.find(f_val);
    if (first == std::string::npos) return -1;
    int l_ret = std::count(f_enum.begin(),f_enum.begin()+first,',');
    return l_ret;
}
unsigned long GetSystemTick()
{
    return GetTickCount();
}

void PathsJoin(std::string &f_path1, std::string &f_path2, std::string &f_out)
{
   f_out.append(f_path1);
   f_out.append("/");
   f_out.append(f_path2);
}

int CompressData(void *f_src, int f_srcLen, void *f_dest, int f_destLen)
{
    z_stream zInfo = {0};
    zInfo.total_in = zInfo.avail_in = f_srcLen;
    zInfo.total_out = zInfo.avail_out = f_destLen;
    zInfo.next_in = (unsigned char*)f_src;
    zInfo.next_out = (unsigned char*)f_dest;

    int l_error, l_ret = -1;
    l_error = deflateInit(&zInfo,Z_DEFAULT_COMPRESSION);
    if(l_error == Z_OK) 
    {
        l_error = deflate(&zInfo,Z_FINISH);
        if(l_error == Z_STREAM_END) l_ret = zInfo.total_out;
    }
    deflateEnd(&zInfo);
    return l_ret;
}
int UncompressData(void *f_src, int f_srcLen, void *f_dest, int f_destLen)
{
    z_stream zInfo = {0};
    zInfo.total_in = zInfo.avail_in = f_srcLen;
    zInfo.total_out = zInfo.avail_out = f_destLen;
    zInfo.next_in = (unsigned char*)f_src;
    zInfo.next_out = (unsigned char*)f_dest;

    int l_error, l_ret = -1;
    l_error = inflateInit(&zInfo);
    if(l_error == Z_OK)
    {
        l_error = inflate(&zInfo,Z_FINISH);
        if(l_error == Z_STREAM_END) l_ret = zInfo.total_out;
    }
    inflateEnd(&zInfo);
    return l_ret;
}
int GetMaxCompressedLen(int nLenSrc)
{
    int n16kBlocks = (nLenSrc+16383)/16384;
    return (nLenSrc+6+(n16kBlocks*5));
}

}
}