#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <cstdlib>
//#include <chrono>

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "Light.h"
#include "PrimeEngine/GameObjectModel/Camera.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

// Sibling/Children includes
#include "ParticlesSystem.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
	namespace Components {

		PE_IMPLEMENT_CLASS1(ParticlesSystem, Mesh);

		void ParticlesSystem::addDefaultComponents()
		{
			//add this handler before Mesh's handlers so we can intercept draw and modify transform
			PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, ParticlesSystem::do_GATHER_DRAWCALLS);
			Mesh::addDefaultComponents();
		}

		float ParticlesSystem::calculatePositonByEquation(float speed, float deltaTime, float partialLife)
		{
			return speed * deltaTime * (partialLife + 1);
		}

		void ParticlesSystem::loadTexture_needsRC(const char* techName, int& threadOwnershipMask)
		{
			//int len = StringOps::length(str);
			
			if (!m_meshCPU.isValid())
			{
				m_meshCPU = Handle("MeshCPU ParticlesSystem", sizeof(MeshCPU));
				new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
			}
			MeshCPU& mcpu = *m_meshCPU.getObject<MeshCPU>();

			if (!m_loaded)
				mcpu.createBillboardMeshWithColorTexture("rgb.dds", "Default", 32, 32, SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp);

			if (!m_loaded)
			{
				// this will cause not using the vertex buffer manager
				//so that engine always creates a new vertex buffer gpu and doesn't try to find and
				//existing one
				mcpu.m_manualBufferManagement = true;

				m_lineColor = Vector3(0.0, 0.5, 0.5);

				PositionBufferCPU* pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
				TexCoordBufferCPU* pTCB = mcpu.m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>();
				NormalBufferCPU* pNB = mcpu.m_hNormalBufferCPU.getObject<NormalBufferCPU>();
				IndexBufferCPU* pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();
				pVB->m_values.reset(m_numParticles * 4 * 3); // 4 verts * (x,y,z)
				pTCB->m_values.reset(m_numParticles * 4 * 2);
				pNB->m_values.reset(m_numParticles * 4 * 3);
				pIB->m_values.reset(m_numParticles * 6); // 2 tris

				pIB->m_indexRanges[0].m_start = 0;
				pIB->m_indexRanges[0].m_end = m_numParticles * 6 - 1;
				pIB->m_indexRanges[0].m_minVertIndex = 0;
				pIB->m_indexRanges[0].m_maxVertIndex = m_numParticles * 4 - 1;

				pIB->m_minVertexIndex = pIB->m_indexRanges[0].m_minVertIndex;
				pIB->m_maxVertexIndex = pIB->m_indexRanges[0].m_maxVertIndex;

				/*float w = 32.f / 2.0f;
				float h = 32.f;
				h = h / w;
				w = 1.0f;*/

				/*m_charW = w;
				m_charH = h;
				m_charWAbs = fabs(w);
				m_textLength = (float)(m_numParticles);
				float curX = 0;
				float curY = 0;
				float pixSize2 = 1.0f / 512.0f / 2.0f;*/
				for (int ic = 0; ic < m_numParticles; ic++)
				{
					//PEINFO("numParticles: %d\n", m_numParticles);
					m_lifeTimeBuffer[ic] = generateRandomFloatNormalized() * m_lifeTime; // actual lifetime is 1 - normalized value
					m_InitialLifeTimeBuffer[ic] = m_lifeTimeBuffer[ic];
					m_velocityBuffer[ic] = getRandomVelocity();
					/*char c = str[ic];
					int row = int(c) / 16;
					int column = int(c) % 16;
					float tcx = 1.0f / 16.0f * float(column);
					float tcy = 1.0f / 16.0f * float(row);*/

					/*m_dimBox[0].m_x = -5;
					m_dimBox[1].m_x = 5;
					m_dimBox[0].m_y = -5;
					m_dimBox[1].m_y = 5;
					m_dimBox[0].m_z = 10;
					m_dimBox[1].m_z = 10;
					Vector3 dimBoxCenter;
					dimBoxCenter.m_x = (m_dimBox[0].m_x + m_dimBox[1].m_x) / 2;
					dimBoxCenter.m_y = (m_dimBox[0].m_y + m_dimBox[1].m_y) / 2;
					dimBoxCenter.m_z = (m_dimBox[0].m_z + m_dimBox[1].m_z) / 2;*/

					Vector3 basePos = getRandomPositionInSpawnerVolumn();
					//basePos = Vector3(0, 0, 0);

					/*pVB->m_values.add(basePos.m_x - m_dim / 2, basePos.m_y + m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x + m_dim / 2, basePos.m_y + m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x + m_dim / 2, basePos.m_y - m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x - m_dim / 2, basePos.m_y - m_dim / 2, basePos.m_z);*/

					pVB->m_values.add(basePos.m_x - m_dim / 2, basePos.m_y + m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x + m_dim / 2, basePos.m_y + m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x + m_dim / 2, basePos.m_y - m_dim / 2, basePos.m_z);
					pVB->m_values.add(basePos.m_x - m_dim / 2, basePos.m_y - m_dim / 2, basePos.m_z);

					/*pVB->m_values.add(-1, 1, 20);
					pVB->m_values.add(1, 1, 20);
					pVB->m_values.add(1, -1, 20);
					pVB->m_values.add(-1, -1, 20);*/
					//PEINFO("pVB:\n %f, %f, %f\n %f, %f, %f\n %f, %f, %f\n %f, %f, %f\n", pVB->m_values[0], pVB->m_values[1], pVB->m_values[2], pVB->m_values[3], pVB->m_values[4], pVB->m_values[5], pVB->m_values[6], pVB->m_values[7], pVB->m_values[8], pVB->m_values[9], pVB->m_values[10], pVB->m_values[11]);

					pIB->m_values.add(ic * 4 + 0, ic * 4 + 1, ic * 4 + 2);
					pIB->m_values.add(ic * 4 + 2, ic * 4 + 3, ic * 4 + 0);

					//float dx = pixSize2;
					//float dy = pixSize2;
					//pTCB->m_values.add(tcx + dx, tcy + dy); // top left
					//pTCB->m_values.add(tcx + 1.0f / 16.0f - dx, tcy + dy); // top right
					//pTCB->m_values.add(tcx + 1.0f / 16.0f - dx, tcy + 1.0f / 16.0f - dy);
					//pTCB->m_values.add(tcx + dx, tcy + 1.0f / 16.0f - dy);
					pTCB->m_values.add(1, 1); // top left
					pTCB->m_values.add(0, 1); // top right
					pTCB->m_values.add(0, 0);
					pTCB->m_values.add(1, 0);

					pNB->m_values.add(0, 0, 0);
					pNB->m_values.add(0, 0, 0);
					pNB->m_values.add(0, 0, 0);
					pNB->m_values.add(0, 0, 0);
					//curX += w;
				}
				m_begin = std::chrono::steady_clock::now();
				// first time creating gpu mesh
				loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

				if (techName)
				{
					Handle hEffect = EffectManager::Instance()->getEffectHandle(techName);

					for (unsigned int imat = 0; imat < m_effects.m_size; imat++)
					{
						if (m_effects[imat].m_size)
							m_effects[imat][0] = hEffect;
					}
				}
				m_loaded = true;
			}
			else
			{
				//just need to update vertex buffers gpu
				//Modified:
				//Update buffer here
				setDeltaTime();
				PositionBufferCPU* pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
				TexCoordBufferCPU* pTCB = mcpu.m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>();
				NormalBufferCPU* pNB = mcpu.m_hNormalBufferCPU.getObject<NormalBufferCPU>();
				IndexBufferCPU* pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();
				for (int ic = 0; ic < m_numParticles; ic++) 
				{
					m_lifeTimeBuffer[ic] += m_deltaTime;
					if (m_lifeTimeBuffer[ic] > m_lifeTime) 
					{
						resetParticleByIndex(ic, pVB, pTCB, pNB, pIB);
					}
					else 
					{
						updateParticleByIndex(ic, pVB, pTCB, pNB, pIB);
					}
					if (m_drawDebugMesh)
					{
						Vector3 point1 = Vector3(pVB->m_values[ic * 12 + 0], pVB->m_values[ic * 12 + 1], pVB->m_values[ic * 12 + 2]);
						Vector3 point2 = Vector3(pVB->m_values[ic * 12 + 3], pVB->m_values[ic * 12 + 4], pVB->m_values[ic * 12 + 5]);
						DebugRenderer::Instance()->createQuadMesh(m_lineColor, point1, point2);
						point1 = Vector3(pVB->m_values[ic * 12 + 3], pVB->m_values[ic * 12 + 4], pVB->m_values[ic * 12 + 5]);
						point2 = Vector3(pVB->m_values[ic * 12 + 6], pVB->m_values[ic * 12 + 7], pVB->m_values[ic * 12 + 8]);
						DebugRenderer::Instance()->createQuadMesh(m_lineColor, point1, point2);
						point1 = Vector3(pVB->m_values[ic * 12 + 6], pVB->m_values[ic * 12 + 7], pVB->m_values[ic * 12 + 8]);
						point2 = Vector3(pVB->m_values[ic * 12 + 9], pVB->m_values[ic * 12 + 10], pVB->m_values[ic * 12 + 11]);
						DebugRenderer::Instance()->createQuadMesh(m_lineColor, point1, point2);
						point1 = Vector3(pVB->m_values[ic * 12 + 9], pVB->m_values[ic * 12 + 10], pVB->m_values[ic * 12 + 11]);
						point2 = Vector3(pVB->m_values[ic * 12 + 0], pVB->m_values[ic * 12 + 1], pVB->m_values[ic * 12 + 2]);
						DebugRenderer::Instance()->createQuadMesh(m_lineColor, point1, point2);
					}
				}
				updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
				
			}
		}

		void ParticlesSystem::do_GATHER_DRAWCALLS(Events::Event* pEvt)
		{

		}

		void ParticlesSystem::resetParticleByIndex(
			int particleIndex, 
			PositionBufferCPU* pVB, TexCoordBufferCPU* pTCB,
			NormalBufferCPU* pNB, IndexBufferCPU* pIB)
		{
			//pVB->m_values.add(curX, curY, 0); // top lef
			//pVB->m_values.add(curX + w, curY, 0); // top right
			//pVB->m_values.add(curX + w, curY - h, 0); // bot right
			//pVB->m_values.add(curX, curY - h, 0); // bot lef

			Vector3 basePos = getRandomPositionInSpawnerVolumn();

			pVB->m_values[particleIndex * 12 + 0] = basePos.m_x - m_dim / 2;
			pVB->m_values[particleIndex * 12 + 1] = basePos.m_y + m_dim / 2;
			pVB->m_values[particleIndex * 12 + 2] = basePos.m_z;

			pVB->m_values[particleIndex * 12 + 3] = basePos.m_x + m_dim / 2;
			pVB->m_values[particleIndex * 12 + 4] = basePos.m_y + m_dim / 2;
			pVB->m_values[particleIndex * 12 + 5] = basePos.m_z;
			
			pVB->m_values[particleIndex * 12 + 6] = basePos.m_x + m_dim / 2;
			pVB->m_values[particleIndex * 12 + 7] = basePos.m_y - m_dim / 2;
			pVB->m_values[particleIndex * 12 + 8] = basePos.m_z;

			pVB->m_values[particleIndex * 12 + 9] = basePos.m_x - m_dim / 2;
			pVB->m_values[particleIndex * 12 + 10] = basePos.m_y - m_dim / 2;
			pVB->m_values[particleIndex * 12 + 11] = basePos.m_z;

			m_velocityBuffer[particleIndex] = getRandomVelocity();
			m_lifeTimeBuffer[particleIndex] = generateRandomFloatNormalized() * m_lifeTime;
		}

		void ParticlesSystem::updateParticleByIndex(
			int particleIndex,
			PositionBufferCPU* pVB, TexCoordBufferCPU* pTCB,
			NormalBufferCPU* pNB, IndexBufferCPU* pIB)
		{
			/*Vector3 tempV = m_velocityBuffer[particleIndex];
			Vector3 tempP = Vector3(pVB->m_values[particleIndex * 3 + 0], pVB->m_values[particleIndex * 3 + 1], pVB->m_values[particleIndex * 3 + 2]);
			PEINFO("Index: %d, Delta: %f, Pos: %f, %f, %f, Vel: %f, %f, %f\n", particleIndex, m_deltaTime, tempP.m_x, tempP.m_y, tempP.m_z, tempV.m_x, tempV.m_y, tempV.m_z);
			
			tempP = Vector3(pVB->m_values[particleIndex * 12 + 0], pVB->m_values[particleIndex * 12 + 1], pVB->m_values[particleIndex * 12 + 2]);
			PEINFO("Pos 0: %f, %f, %f\n", tempP.m_x, tempP.m_y, tempP.m_z);
			tempP = Vector3(pVB->m_values[particleIndex * 12 + 3], pVB->m_values[particleIndex * 12 + 4], pVB->m_values[particleIndex * 12 + 5]);
			PEINFO("Pos 1: %f, %f, %f\n", tempP.m_x, tempP.m_y, tempP.m_z);
			tempP = Vector3(pVB->m_values[particleIndex * 12 + 6], pVB->m_values[particleIndex * 12 + 7], pVB->m_values[particleIndex * 12 + 8]);
			PEINFO("Pos 2: %f, %f, %f\n", tempP.m_x, tempP.m_y, tempP.m_z);
			tempP = Vector3(pVB->m_values[particleIndex * 12 + 9], pVB->m_values[particleIndex * 12 + 10], pVB->m_values[particleIndex * 12 + 11]);
			PEINFO("Pos 3: %f, %f, %f\n", tempP.m_x, tempP.m_y, tempP.m_z);*/
			float partialLife = (m_lifeTimeBuffer[particleIndex] - m_InitialLifeTimeBuffer[particleIndex]) / (m_lifeTime - m_InitialLifeTimeBuffer[particleIndex]);
			
			pVB->m_values[particleIndex * 12 + 0] += m_velocityBuffer[particleIndex].m_x * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 1] += m_velocityBuffer[particleIndex].m_y * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 2] += m_velocityBuffer[particleIndex].m_z * m_deltaTime;
			
			pVB->m_values[particleIndex * 12 + 3] += m_velocityBuffer[particleIndex].m_x * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 4] += m_velocityBuffer[particleIndex].m_y * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 5] += m_velocityBuffer[particleIndex].m_z * m_deltaTime;

			pVB->m_values[particleIndex * 12 + 6] += m_velocityBuffer[particleIndex].m_x * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 7] += m_velocityBuffer[particleIndex].m_y * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 8] += m_velocityBuffer[particleIndex].m_z * m_deltaTime;

			pVB->m_values[particleIndex * 12 + 9] += m_velocityBuffer[particleIndex].m_x * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 10] += m_velocityBuffer[particleIndex].m_y * m_deltaTime;
			pVB->m_values[particleIndex * 12 + 11] += m_velocityBuffer[particleIndex].m_z * m_deltaTime;
		}

		Vector3 ParticlesSystem::getRandomPositionInSpawnerVolumn() 
		{
			m_dimBox[0].m_x = 0;
			m_dimBox[1].m_x = 0;
			m_dimBox[0].m_y = 0;
			m_dimBox[1].m_y = 0;
			m_dimBox[0].m_z = 0;
			m_dimBox[1].m_z = 0;
			Vector3 dimBoxCenter, dimBoxRange;
			dimBoxCenter.m_x = (m_dimBox[0].m_x + m_dimBox[1].m_x) / 2;
			dimBoxCenter.m_y = (m_dimBox[0].m_y + m_dimBox[1].m_y) / 2;
			dimBoxCenter.m_z = (m_dimBox[0].m_z + m_dimBox[1].m_z) / 2;
			dimBoxRange.m_x = (m_dimBox[1].m_x - m_dimBox[0].m_x) / 2;
			dimBoxRange.m_y = (m_dimBox[1].m_y - m_dimBox[0].m_y) / 2;
			dimBoxRange.m_z = (m_dimBox[1].m_z - m_dimBox[0].m_z) / 2;

			//PrimitiveTypes::Float32 boxSize = 1;
			Vector3 position;
			position.m_x = generateRandomFloatNormalized() * dimBoxRange.m_x + m_dimBox[0].m_x;
			position.m_y = generateRandomFloatNormalized() * dimBoxRange.m_y + m_dimBox[0].m_y;
			position.m_z = generateRandomFloatNormalized() * dimBoxRange.m_z + m_dimBox[0].m_z;
			return position;
		}

		Vector3 ParticlesSystem::getRandomVelocity()
		{
			//int velocityScale = 10;
			m_velocityMultiplier.m_x = 5;
			m_velocityMultiplier.m_y = 50;
			m_velocityMultiplier.m_z = 5;

			Vector3 velocity;
			velocity.m_x = generateRandomFloatNormalized() -0.5;
			velocity.m_y = generateRandomFloatNormalized();// -0.5;
			velocity.m_z = generateRandomFloatNormalized() -0.5;

			velocity.m_x = velocity.m_x * m_velocityMultiplier.m_x;
			velocity.m_y = velocity.m_y * m_velocityMultiplier.m_y;
			velocity.m_z = velocity.m_z * m_velocityMultiplier.m_z;

			return velocity;
			//return Vector3(0, 0, 10);
		}

		PrimitiveTypes::Float32 ParticlesSystem::generateRandomFloatNormalized() 
		{
			//int max = 1000;
			//int seed = getRandomSeed();
			////srand(seed);
			//PrimitiveTypes::Float32 rngFloat = float(rand() % max) / max;
			////PEINFO("Seed: %d, RNG: %f\n", seed, rngFloat);
			//return rngFloat;
			m_counter += 1;
			if (m_counter >= 200) {
				m_counter = 0;
			}
			return float(m_randomList[m_counter]);
		}

		PrimitiveTypes::Float32 ParticlesSystem::setDeltaTime()
		{
			std::chrono::time_point<std::chrono::steady_clock> curr = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = curr - m_begin;
			m_begin = curr;
			m_deltaTime = diff.count();
			m_deltaTime = 0.03;
			return diff.count();
		}

		int ParticlesSystem::getRandomSeed()
		{
			std::chrono::time_point<std::chrono::steady_clock> curr = std::chrono::steady_clock::now();
			int seed = std::chrono::duration_cast<std::chrono::nanoseconds>(curr.time_since_epoch()).count();
			return 124;//seed;
		}

	}; // namespace Components
}; // namespace PE
