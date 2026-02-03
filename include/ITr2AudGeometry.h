////////////////////////////////////////////////////////////
//
//    Creator:   Phevos Rinis
//    Created:   Jan 2026
//
//    Description:
//      An interface intended for use by audio2 to expose relevant functions
//      related to audio emitters.


#pragma once

#ifndef ITr2AudGeometry_h_
#define ITr2AudGeometry_h_

struct Tr2AudGeometryData
{
	std::vector<Vector3> m_vertices;
	std::vector<uint32_t> m_indices;
	Vector3 m_minBounds;
	Vector3 m_maxBounds;
};

BLUE_INTERFACE( ITr2AudGeometry ) :
	public IRoot
{
	virtual void SetGeometry(
		uint64_t geometryId,
		const Tr2AudGeometryData& geometryData,
		const Matrix& worldTransform ) = 0;

	virtual void SetGeometryTransform(
		uint64_t geometryId,
		const Matrix& worldTransform ) = 0;

	virtual void RemoveGeometry( uint64_t geometryId ) = 0;
};

#endif