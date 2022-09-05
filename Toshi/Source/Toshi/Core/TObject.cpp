#include "ToshiPCH.h"
#include "TObject.h"

namespace Toshi
{
	// static initialization of TObject's TClass
	constinit TClassImpl TObject::s_Class
	{
		"TObject",                              // m_Name
		TObject::CreateTObject,                 // m_Create
		TObject::CreateTObjectInPlace,          // m_CreateInPlace
		nullptr,                                // m_Initialize
		nullptr,                                // m_Uninitialize
		nullptr,                                // m_Parent
		nullptr,                                // m_Previous
		nullptr,                                // m_LastAttached,
		TMAKEVERSION(1, 0),                     // m_Version
		sizeof(TObject),                        // m_Size
		0,                                      // m_Unk
		false                                   // m_Initialized
	};
}