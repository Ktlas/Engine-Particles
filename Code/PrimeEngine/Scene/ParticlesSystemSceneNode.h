#ifndef __PYENGINE_2_0_PARTICLESSYSTEMSCENENODE_H__
#define __PYENGINE_2_0_PARTICLESSYSTEMSCENENODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "SceneNode.h"

//#define USE_DRAW_COMPONENT

namespace PE {
	namespace Components {
		struct ParticlesSystemSceneNode : public SceneNode
		{
			PE_DECLARE_CLASS(ParticlesSystemSceneNode);

			// Constructor -------------------------------------------------------------
			ParticlesSystemSceneNode(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);

			virtual ~ParticlesSystemSceneNode() {}

			void setSelfAndMeshAssetEnabled(bool enabled);

			// Component ------------------------------------------------------------

			virtual void addDefaultComponents();

			// Individual events -------------------------------------------------------

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_GATHER_DRAWCALLS);
			virtual void do_PRE_GATHER_DRAWCALLS(Events::Event* pEvt);

			enum DrawType
			{
				InWorld,
				InWorldFacingCamera,
				Overlay2D,
				Overlay2D_3DPos
			};
			void loadTexture_needsRC(DrawType drawType, int& threadOwnershipMask);

			DrawType m_drawType;
			float m_scale;
			Handle m_hMyParticlesSystem;
			Handle m_hMyParticlesSystemInstance;
			float m_cachedAspectRatio;

			bool m_canBeRecreated;

		}; // class ParticlesSystemSceneNode

	}; // namespace Components
}; // namespace PE
#endif
