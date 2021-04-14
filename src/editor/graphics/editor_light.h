#pragma once

namespace Editor {

	struct ELight
	{
		EVec3 Direction;
		EVec3 Radiance;
	};

	typedef std::vector<ELight> ELightMap;

}