////////////////////////////////////////////////////////////
//
//    Creator:   CCP Team Engine
//    Created:   April 2020
//    Modified:  March 2025
//    Copyright (c) 2026 CCP Games
//
//    Description:
//      An interface allowing for an abstraction of audio on an asset.
//      Note: This interface is deprecated. Please use IStretchAudio instead.

#pragma once

#include "ITr2AudEmitter.h"

BLUE_INTERFACE( ITr2Audio ) : public IRoot
{
	virtual void Update( Vector3& sourcePosition, Vector3& destPosition) = 0;
	virtual ITr2AudEmitterPtr FindEmitterByName( const char* name ) = 0;
};
