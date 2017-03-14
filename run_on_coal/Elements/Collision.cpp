#include "stdafx.h"
#include "Elements/Collision.h"

ROC::Collision::Collision()
{
    m_elementType = ElementType::CollisionElement;
    m_rigidBody = NULL;
    m_type = COLLISION_TYPE_NONE;
}
ROC::Collision::~Collision()
{
    if(m_rigidBody)
    {
        delete m_rigidBody->getMotionState();
        delete m_rigidBody;
    }
}

bool ROC::Collision::Create(int f_type, const glm::vec3 &f_size)
{
    if(!m_rigidBody)
    {
        m_type = f_type;
        btClamp(m_type, COLLISION_TYPE_SPHERE, COLLISION_TYPE_CONE);
        btCollisionShape *l_shape = NULL;
        switch(m_type)
        {
            case COLLISION_TYPE_SPHERE:
                l_shape = new btSphereShape(f_size.x);
                break;
            case COLLISION_TYPE_BOX:
                l_shape = new btBoxShape(btVector3(f_size.x, f_size.y, f_size.z));
                break;
            case COLLISION_TYPE_CYLINDER:
                l_shape = new btCylinderShape(btVector3(f_size.x, f_size.y, f_size.z));
                break;
            case COLLISION_TYPE_CAPSULE:
                l_shape = new btCapsuleShape(f_size.x, f_size.y);
                break;
            case COLLISION_TYPE_CONE:
                l_shape = new btConeShape(f_size.x, f_size.y);
                break;
        }
        btTransform l_transform;
        l_transform.setIdentity();
        btDefaultMotionState *l_fallMotionState = new btDefaultMotionState(l_transform);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(0.f, l_fallMotionState, l_shape);
        m_rigidBody = new btRigidBody(fallRigidBodyCI);
    }
    return (m_rigidBody != NULL);
}

void ROC::Collision::GetPosition(glm::vec3 &f_pos)
{
    std::memcpy(&f_pos, m_rigidBody->getCenterOfMassTransform().getOrigin().m_floats, sizeof(glm::vec3));
}
void ROC::Collision::SetPosition(const glm::vec3 &f_pos)
{
    btTransform l_transform = m_rigidBody->getCenterOfMassTransform();
    l_transform.setOrigin(btVector3(f_pos.x,f_pos.y,f_pos.z));
    m_rigidBody->setCenterOfMassTransform(l_transform);
    m_rigidBody->activate(true);
}
void ROC::Collision::GetRotation(glm::quat &f_rot)
{
    btQuaternion l_rotation = m_rigidBody->getCenterOfMassTransform().getRotation();
    f_rot.x = l_rotation.x();
    f_rot.y = l_rotation.y();
    f_rot.z = l_rotation.z();
    f_rot.w = l_rotation.w();
}
void ROC::Collision::SetRotation(const glm::quat &f_rot)
{
    btTransform l_transform = m_rigidBody->getCenterOfMassTransform();
    l_transform.setRotation((btQuaternion&)f_rot);
    m_rigidBody->setCenterOfMassTransform(l_transform);
    m_rigidBody->activate(true);
}
