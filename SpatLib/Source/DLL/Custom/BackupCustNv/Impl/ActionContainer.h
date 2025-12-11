#pragma once
#include "RuntimeContainer.hpp"

/// Example: Bind the Action Name by defining SpatContainer::CRuntimeClassContainer object
/// SpatContainer::CRuntimeClassContainer RTC_CReadSN( L"CReadSN", L"CReadSN1" );
/// SpatContainer::CRuntimeClassContainer RTC_CSleep( L"CSleep", L"CSleep1",L"CSleep2" );

/// Example: Bind the Action Name specified parameters by defining SpatContainer::CRuntimeClassContainer object
/// SpatContainer::CRuntimeParamsContainer RTP_CReadSN_SN1( L"CReadSN", L"Option:SN1", L"Option:SN" );
/// SpatContainer::CRuntimeParamsContainer RTP_CReadSN_SN2( L"CReadSN", L"Option:SN2", L"Option:SN21", L"Option:SN22" );

/// Example: Obtain parameters through multiple configuration paths
///BOOL CReadSN::LoadXMLConfig(void)
///{
///	//m_bSN1 = GetConfigValue(L"Option:SN1", 1);
///	SpatParamsPath old_paths = g_SpatParamsContainer.FindAllPath(L"CReadSN", L"Option:SN1");
///	for (auto it : old_paths)
///	{
///		m_bSN1 = GetConfigValue(it.c_str(), 1);
///	}
///	return TRUE;
///}
