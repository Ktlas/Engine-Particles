#include "CameraSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"

#define Z_ONLY_CAM_BIAS 0.0f
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(CameraSceneNode, SceneNode);

CameraSceneNode::CameraSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : SceneNode(context, arena, hMyself)
{
	m_near = 0.05f;
	m_far = 2000.0f;
}
void CameraSceneNode::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, CameraSceneNode::do_CALCULATE_TRANSFORMATIONS);
}

void CameraSceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hParentSN = getFirstParentByType<SceneNode>();
	if (hParentSN.isValid())
	{
		Matrix4x4 parentTransform = hParentSN.getObject<PE::Components::SceneNode>()->m_worldTransform;
		m_worldTransform = parentTransform * m_base;
	}
	
	Matrix4x4 &mref_worldTransform = m_worldTransform;

	Vector3 pos = Vector3(mref_worldTransform.m[0][3], mref_worldTransform.m[1][3], mref_worldTransform.m[2][3]);
	Vector3 n = Vector3(mref_worldTransform.m[0][2], mref_worldTransform.m[1][2], mref_worldTransform.m[2][2]);
	Vector3 target = pos + n;
	Vector3 up = Vector3(mref_worldTransform.m[0][1], mref_worldTransform.m[1][1], mref_worldTransform.m[2][1]);

	m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

	m_worldTransform2 = mref_worldTransform;

	m_worldTransform2.moveForward(Z_ONLY_CAM_BIAS);

	Vector3 pos2 = Vector3(m_worldTransform2.m[0][3], m_worldTransform2.m[1][3], m_worldTransform2.m[2][3]);
	Vector3 n2 = Vector3(m_worldTransform2.m[0][2], m_worldTransform2.m[1][2], m_worldTransform2.m[2][2]);
	Vector3 target2 = pos2 + n2;
	Vector3 up2 = Vector3(m_worldTransform2.m[0][1], m_worldTransform2.m[1][1], m_worldTransform2.m[2][1]);

	m_worldToViewTransform2 = CameraOps::CreateViewMatrix(pos2, target2, up2);
    
    PrimitiveTypes::Float32 aspect = (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getWidth()) / (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getHeight());
    
    PrimitiveTypes::Float32 verticalFov = 0.33f * PrimitiveTypes::Constants::c_Pi_F32;
    if (aspect < 1.0f)
    {
        //ios portrait view
        static PrimitiveTypes::Float32 factor = 0.5f;
        verticalFov *= factor;
    }

	m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(verticalFov, 
		aspect,
		m_near, m_far);
	
	

	//Narrow fov for debugging
	PrimitiveTypes::Float32 debugFov = 0.6f * PrimitiveTypes::Constants::c_Pi_F32;
	Matrix4x4 m_debugViewToProjectedTransform = CameraOps::CreateProjectionMatrix(debugFov, aspect, m_near, m_far);
	Matrix4x4 m_mvp = m_debugViewToProjectedTransform * m_worldToViewTransform;
	m_mvp = m_mvp.transpose();

	//Default fov
	/*Matrix4x4 m_mvp = m_viewToProjectedTransform * m_worldToViewTransform;
	m_mvp = m_mvp.transpose();*/
	
	SceneNode::do_CALCULATE_TRANSFORMATIONS(pEvt);
	

	// Create Frustum Planes
	// Left
	m_camFrustum.m_frustum[0].m_A = m_mvp.m[0][3] + m_mvp.m[0][0];
	m_camFrustum.m_frustum[0].m_B = m_mvp.m[1][3] + m_mvp.m[1][0];
	m_camFrustum.m_frustum[0].m_C = m_mvp.m[2][3] + m_mvp.m[2][0];
	m_camFrustum.m_frustum[0].m_D = m_mvp.m[3][3] + m_mvp.m[3][0];
	// Right
	m_camFrustum.m_frustum[1].m_A = m_mvp.m[0][3] - m_mvp.m[0][0];
	m_camFrustum.m_frustum[1].m_B = m_mvp.m[1][3] - m_mvp.m[1][0];
	m_camFrustum.m_frustum[1].m_C = m_mvp.m[2][3] - m_mvp.m[2][0];
	m_camFrustum.m_frustum[1].m_D = m_mvp.m[3][3] - m_mvp.m[3][0];
	// Bottom
	m_camFrustum.m_frustum[2].m_A = m_mvp.m[0][3] + m_mvp.m[0][1];
	m_camFrustum.m_frustum[2].m_B = m_mvp.m[1][3] + m_mvp.m[1][1];
	m_camFrustum.m_frustum[2].m_C = m_mvp.m[2][3] + m_mvp.m[2][1];
	m_camFrustum.m_frustum[2].m_D = m_mvp.m[3][3] + m_mvp.m[3][1];
	// Top
	m_camFrustum.m_frustum[3].m_A = m_mvp.m[0][3] - m_mvp.m[0][1];
	m_camFrustum.m_frustum[3].m_B = m_mvp.m[1][3] - m_mvp.m[1][1];
	m_camFrustum.m_frustum[3].m_C = m_mvp.m[2][3] - m_mvp.m[2][1];
	m_camFrustum.m_frustum[3].m_D = m_mvp.m[3][3] - m_mvp.m[3][1];
	// Near
	m_camFrustum.m_frustum[4].m_A = m_mvp.m[0][3] + m_mvp.m[0][2];
	m_camFrustum.m_frustum[4].m_B = m_mvp.m[1][3] + m_mvp.m[1][2];
	m_camFrustum.m_frustum[4].m_C = m_mvp.m[2][3] + m_mvp.m[2][2];
	m_camFrustum.m_frustum[4].m_D = m_mvp.m[3][3] + m_mvp.m[3][2];
	// Far
	m_camFrustum.m_frustum[5].m_A = m_mvp.m[0][3] - m_mvp.m[0][2];
	m_camFrustum.m_frustum[5].m_B = m_mvp.m[1][3] - m_mvp.m[1][2];
	m_camFrustum.m_frustum[5].m_C = m_mvp.m[2][3] - m_mvp.m[2][2];
	m_camFrustum.m_frustum[5].m_D = m_mvp.m[3][3] - m_mvp.m[3][2];
}

bool CameraSceneNode::checkAABBIntersectsFrustum(Vector3 minTransformed, Vector3 maxTransformed)
{
	// Reference: https://www.gamedev.net/forums/topic/512123-fast--and-correct-frustum---aabb-intersection/
	// Reference: https://gist.github.com/Kinwailo/d9a07f98d8511206182e50acda4fbc9b
	// These two use an optimized method, but not suitable for the task
	bool isInside = true;

	//Vector3 minTransformed = transMat * mins;
	//Vector3 maxTransformed = transMat * maxs;
	Vector3 vMin, vMax;
	Vector3 vertices[8];
	vertices[0] = { minTransformed.m_x, minTransformed.m_y, minTransformed.m_z };
	vertices[1] = { maxTransformed.m_x, minTransformed.m_y, minTransformed.m_z };
	vertices[2] = { minTransformed.m_x, maxTransformed.m_y, minTransformed.m_z };
	vertices[3] = { minTransformed.m_x, minTransformed.m_y, maxTransformed.m_z };
	vertices[4] = { maxTransformed.m_x, maxTransformed.m_y, maxTransformed.m_z };
	vertices[5] = { minTransformed.m_x, maxTransformed.m_y, maxTransformed.m_z };
	vertices[6] = { maxTransformed.m_x, minTransformed.m_y, maxTransformed.m_z };
	vertices[7] = { maxTransformed.m_x, maxTransformed.m_y, minTransformed.m_z };
	for (int j = 0; j < 8; j++)
	{
		int insidePlaneCounter = 0;
		for (int i = 0; i < 6; i++) //there are 6 planes
		{
			////x axis
			//if (m_camFrustum.m_frustum[i].m_A > 0) 
			//{
			//	vMin.m_x = minTransformed.m_x;
			//	vMax.m_x = maxTransformed.m_x;
			//}
			//else 
			//{
			//	vMin.m_x = maxTransformed.m_x;
			//	vMax.m_x = minTransformed.m_x;
			//}
			////y axis
			//if (m_camFrustum.m_frustum[i].m_B > 0) 
			//{
			//	vMin.m_y = minTransformed.m_y;
			//	vMax.m_y = maxTransformed.m_y;
			//}
			//else 
			//{
			//	vMin.m_y = maxTransformed.m_y;
			//	vMax.m_y = minTransformed.m_y;
			//}
			////z axis
			//if (m_camFrustum.m_frustum[i].m_C > 0) 
			//{
			//	vMin.m_z = minTransformed.m_z;
			//	vMax.m_z = maxTransformed.m_z;
			//}
			//else
			//{
			//	vMin.m_z = maxTransformed.m_z;
			//	vMax.m_z = minTransformed.m_z;
			//}
			Vector3 planeNormal = { m_camFrustum.m_frustum[i].m_A, m_camFrustum.m_frustum[i].m_B, m_camFrustum.m_frustum[i].m_C };

			
			if (planeNormal.dotProduct(vertices[j]) + m_camFrustum.m_frustum[i].m_D >= 0) //inside plane
			{
				insidePlaneCounter += 1;
			}
			//PEINFO("Debug message from aabb check: %f\n", planeNormal.dotProduct(vMin) + m_camFrustum.m_frustum[i].m_D);
			//isInside = isInside && (planeNormal.dotProduct(vMin) + m_camFrustum.m_frustum[i].m_D >= 0);
		}
		if (insidePlaneCounter == 6) {
			// this vertex is inside the frustum, the mesh with aabb that contains this vertex should be send to the pipeline
			return true;
		}
	}
	return false;
	//PEINFO("Debug message from aabb check: %d\n", 0);
	//return true;
	//return isInside;
}

}; // namespace Components
}; // namespace PE
