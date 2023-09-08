#ifndef __PYENGINE_2_0_PARTICLESSYSTEM_H__
#define __PYENGINE_2_0_PARTICLESSYSTEM_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <chrono>
#include <vector>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
	namespace Components {

		struct ParticlesSystem : public Mesh
		{
			PE_DECLARE_CLASS(ParticlesSystem);

			// Constructor -------------------------------------------------------------
			ParticlesSystem(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself) : Mesh(context, arena, hMyself)
			{
				m_loaded = false;
			}

			virtual ~ParticlesSystem() {}

			virtual void addDefaultComponents();

			virtual float calculatePositonByEquation(float speed, float deltaTime, float partialLife);

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS);
			virtual void do_GATHER_DRAWCALLS(Events::Event* pEvt);

			void resetParticleByIndex(int particleIndex, PositionBufferCPU* pVB, TexCoordBufferCPU* pTCB, NormalBufferCPU* pNB, IndexBufferCPU* pIB);

			void updateParticleByIndex(int particleIndex, PositionBufferCPU* pVB, TexCoordBufferCPU* pTCB, NormalBufferCPU* pNB, IndexBufferCPU* pIB);

			Vector3 getRandomPositionInSpawnerVolumn();

			Vector3 getRandomVelocity();

			PrimitiveTypes::Float32 generateRandomFloatNormalized();

			PrimitiveTypes::Float32 setDeltaTime();

			int getRandomSeed();

			void loadTexture_needsRC(const char* techName, int& threadOwnershipMask);

			PrimitiveTypes::Float32 m_charW, m_charWAbs, m_charH, m_textLength;
			PrimitiveTypes::Bool m_loaded;
			Handle m_meshCPU;

			PrimitiveTypes::Float32 m_lifeTime = 3.0;
			PrimitiveTypes::Int16 m_numParticles = 50;
			Vector3 m_velocityBuffer[200];
			PrimitiveTypes::Float32 m_lifeTimeBuffer[200];
			PrimitiveTypes::Float32 m_InitialLifeTimeBuffer[200];
			PrimitiveTypes::Float32 m_dim = 1;
			Vector3 m_dimBox[2];
			Vector3 m_velocityMultiplier;

			PrimitiveTypes::Float32 m_deltaTime = 0.0;
			std::chrono::time_point<std::chrono::steady_clock> m_begin;

			PrimitiveTypes::Int16 m_counter = 0;
			std::vector<double> m_randomList{ 0.93127,0.68774,0.22898,0.25498,0.78167,0.38535,0.31259,0.83586,0.40416,0.90383,0.07347,0.48958,0.23382,0.12453,0.70492,0.94018,0.75837,0.40834,0.84514,0.28922,0.95007,0.08530,0.87019,0.76433,0.60194,0.46633,0.83369,0.43780,0.59075,0.79960,0.91948,0.94782,0.10097,0.25205,0.39257,0.84429,0.91346,0.80244,0.50631,0.06798,0.96789,0.13325,0.16923,0.77862,0.80286,0.41173,0.48472,0.30288,0.01768,0.14500,0.07491,0.07392,0.93321,0.92168,0.18714,0.54930,0.83335,0.69101,0.62854,0.89331,0.60886,0.37274,0.58745,0.51743,0.66787,0.89028,0.83041,0.03366,0.70773,0.84664,0.83824,0.61706,0.72810,0.02098,0.04204,0.10683,0.32234,0.92050,0.38232,0.17479,0.88631,0.58457,0.83700,0.27065,0.41468,0.01742,0.24768,0.66897,0.69860,0.00937,0.16878,0.04674,0.40715,0.54434,0.63222,0.91476,0.02816,0.80323,0.61841,0.41577,0.81775,0.37727,0.41277,0.65803,0.47704,0.02595,0.83746,0.24173,0.75330,0.44586,0.15696,0.09055,0.26361,0.84371,0.10763,0.29499,0.81871,0.90742,0.13506,0.98283,0.02952,0.98159,0.34102,0.97896,0.19994,0.01208,0.54866,0.84831,0.84280,0.54392,0.94301,0.27832,0.78732,0.74340,0.91764,0.61789,0.49878,0.41320,0.24045,0.67575,0.96420,0.55315,0.96071,0.38427,0.13116,0.76624,0.69626,0.34236,0.02366,0.98041,0.30381,0.96013,0.20545,0.32863,0.13783,0.04860,0.45045,0.18395,0.53839,0.57916,0.97467,0.07467,0.82630,0.62151,0.22499,0.06890,0.92188,0.10919,0.16363,0.07843,0.85749,0.58592,0.96473,0.70691,0.06109,0.03713,0.83982,0.02520,0.87996,0.19111,0.72743,0.30105,0.84025,0.36957,0.74484,0.29827,0.06907,0.12972,0.36046,0.96446,0.02785,0.12709,0.97340,0.19369,0.30246,0.46289,0.20398,0.57714,0.83022,0.02315 };
			bool m_drawDebugMesh = true;
			Vector3 m_lineColor;
		};

	}; // namespace Components
}; // namespace PE
#endif
