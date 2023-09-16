#pragma once
#include "TSDLWindow.h"

#include "Toshi/Render/TRender.h"
#include "Toshi/Render/TRenderAdapter.h"
#include "Toshi/Render/Shaders/TToneMap.h"
#include "Toshi/Render/Shaders/TFXAA.h"
#include "Toshi/Render/TOrderTable.h"
#include "Toshi2/T2RedBlackTree.h"
#include "Toshi2/T2Pair.h"

namespace Toshi {

	class TRenderSDL :
		public TGenericClassDerived<TRenderSDL, TRender, "TRenderSDL", TMAKEVERSION(1, 0), TFALSE>
	{
	public:
		TRenderSDL() = default;

		virtual TBOOL CreateDisplay(DisplayParams* a_pDisplayParams) override;
		virtual TBOOL DestroyDisplay() override { return TTRUE; }
		virtual void Update(float a_fDeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void* GetCurrentDevice() override { return TNULL; }
		virtual DisplayParams* GetCurrentDisplayParams() override { return &m_DisplayParams; }
		virtual TBOOL Create() override { return Create("TOSHI SDL Application"); }

		virtual TBOOL RecreateDisplay(DisplayParams* a_pDisplayParams);
		virtual void ShowDeviceError();
		virtual void ShowDisplayError();

		TBOOL Create(const char* a_szName);

		TSDLWindow& GetWindow()
		{
			return m_Window;
		}

		TPriList<TOrderTable>& GetOrderTables()
		{
			return m_OrderTables;
		}

	public:
		static TRenderSDL* Interface()
		{
			return static_cast<TRenderSDL*>(TRender::GetSingleton());
		}

	private:
		TSDLWindow m_Window;
		uint32_t m_uiNumDrawnFrames = 0;
		DisplayParams m_DisplayParams;
		TPriList<TOrderTable> m_OrderTables;
		HACCEL m_hAccel = TNULL;
		SDL_GLContext m_GLContext = TNULL;
	};

}
