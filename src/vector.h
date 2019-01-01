#pragma once

struct CVector2f {
    float x, y;
};

struct CVector3f {
    float x, y, z;
};

struct CVector4f {
    float x, y, z, w;

	static CVector4f FromARGB(const uint32_t hex)
	{
        CVector4f result;
        result.w = ((hex >> 24) & 0xFF) / 255.0f;
        result.x = ((hex >> 16) & 0xFF) / 255.0f;
        result.y = ((hex >> 8) & 0xFF) / 255.0f;
        result.z = (hex & 0xFF) / 255.0f;
        return result;
	}
};

struct CMatrix4f {
    CVector4f m[4];
};
