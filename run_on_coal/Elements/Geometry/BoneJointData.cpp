#include "stdafx.h"
#include "Elements/Geometry/BoneJointData.h"

ROC::BoneJointData::BoneJointData()
{
    m_boneID = 0U;
}
ROC::BoneJointData::~BoneJointData()
{
    for(auto iter : m_jointPartVector) delete iter;
    m_jointPartVector.clear();
}

void ROC::BoneJointData::AddPart(unsigned int f_bone, unsigned char f_type, const glm::vec3 &f_size, const glm::vec3 &f_pos, const glm::quat &f_rot, float f_mass, float f_restutition, float f_friction, const glm::vec2 &f_damping, const glm::vec3 &f_lAL, const glm::vec3 &f_uAL, const glm::vec3 &f_aStiffness, const glm::vec3 &f_lLL, const glm::vec3 &f_uLL, const glm::vec3 &f_lStiffness)
{
    bjdJointPart *l_part = new bjdJointPart();
    l_part->m_boneID = f_bone;
    l_part->m_type = f_type;
    std::memcpy(&l_part->m_size, &f_size, sizeof(glm::vec3));
    std::memcpy(&l_part->m_offset, &f_pos, sizeof(glm::vec3));
    std::memcpy(&l_part->m_rotation, &f_rot, sizeof(glm::quat));
    l_part->m_mass = f_mass;
    l_part->m_friction = f_friction;
    l_part->m_restutition = f_restutition;
    std::memcpy(&l_part->m_damping, &f_damping, sizeof(glm::vec2));
    std::memcpy(&l_part->m_lowerAngularLimit, &f_lAL, sizeof(glm::vec3));
    std::memcpy(&l_part->m_upperAngularLimit, &f_uAL, sizeof(glm::vec3));
    std::memcpy(&l_part->m_angularStiffness, &f_aStiffness, sizeof(glm::vec3));
    std::memcpy(&l_part->m_lowerLinearLimit, &f_lLL, sizeof(glm::vec3));
    std::memcpy(&l_part->m_upperLinearLimit, &f_uLL, sizeof(glm::vec3));
    std::memcpy(&l_part->m_linearStiffness, &f_lStiffness, sizeof(glm::vec3));
    m_jointPartVector.push_back(l_part);
}
