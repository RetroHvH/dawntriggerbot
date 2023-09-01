#include "DD.h"

CDD::CDD(void)
{
	m_hModule = NULL;
}

CDD::~CDD(void)
{
	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
	}
}