#ifndef __PYENGINE_2_0_DEBUGRENDERER_H__
#define __PYENGINE_2_0_DEBUGRENDERER_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/SceneNode.h"

// Sibling/Children includes
namespace PE {
	namespace Components {

		struct ParticlesSpawner : public SceneNode
		{
			PE_DECLARE_CLASS(ParticlesSpawner);
			// Singleton ------------------------------------------------------------------

			static void Construct(PE::GameContext& context, PE::MemoryArena arena);

			static ParticlesSpawner* Instance()
			{
				return s_myHandle.getObject<ParticlesSpawner>();
			}

			static Handle InstanceHandle()
			{
				return s_myHandle;
			}

			static void SetInstanceHandle(const Handle& handle)
			{
				// Singleton
				ParticlesSpawner::s_myHandle = handle;
			}

			// Constructor -------------------------------------------------------------
			ParticlesSpawner(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);
			virtual ~ParticlesSpawner() {}
			// Methods      ------------------------------------------------------------

			void createTextMesh(const char* str, bool isOverlay2D, bool is3D, bool is3DFacedToCamera,
				bool is3DFacedToCameraLockedYAxis, float timeToLive, Vector3 pos, float scale,
				int& threadOwnershipMask);
			void createRootLineMesh();
			void createLineMesh(bool hasTransform, const Matrix4x4& transform, float* pRawData, int numInRawData, float timeToLive, float scale = 1.0f);

			void createBoundingVolumeMesh(Vector3 mins, Vector3 maxs);

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_GATHER_DRAWCALLS);
			virtual void do_PRE_GATHER_DRAWCALLS(Events::Event* pEvt);

			void postPreDraw(int& threadOwnershipMask);
			// Component ------------------------------------------------------------

			virtual void addDefaultComponents();
			// Individual events -------------------------------------------------------

		private:
			static Handle s_myHandle;
			Handle m_hMyTextMesh;
			static const int NUM_TextSceneNodes = 64;
			Handle m_hSNPool[NUM_TextSceneNodes];
			int m_hFreeingSNs[NUM_TextSceneNodes];
			int m_hAvailableSNs[NUM_TextSceneNodes];
			float m_lifetimes[NUM_TextSceneNodes];
			int m_numAvaialble;
			int m_numFreeing;


			Handle m_hLineMeshes[2]; // we will alternate between two meshes so that we can generate new one while old one is in draw call
			Handle m_hLineMeshInstances[2];
			int m_currentlyDrawnLineMesh;
			static const int NUM_LineLists = (10 * 1024);
			Array<Array<float> > m_lineLists;
			float m_lineListLifetimes[NUM_LineLists];
			int m_availableLineLists[NUM_LineLists];
			int m_numAvailableLineLists;

		};

	}; // namespace Components
}; // namespace PE
#endif
