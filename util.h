#ifndef UTIL_H
#define UTIL_H

#include <cmath>

float lerp(float v0, float v1, float t) {
    return (1-t)*v0 + t*v1;
}

float get3D(float X, float Y, float Z, float eX, float eY, float eZ)	
{
	return(sqrtf( (eX - X) * (eX - X) + (eY - Y) * (eY - Y) + (eZ - Z) * (eZ - Z)));
}

void calcAngle( float *src, float *dst, float *angles , int fFlags)
{
	double delta[3] = { (src[0]-dst[0]), (src[1]-dst[1]), (src[2]-dst[2]) };
	double hyp = sqrt(delta[0]*delta[0] + delta[1]*delta[1]);
	angles[0] = (float) (asinf(delta[2]/hyp) * 57.295779513082f);
	angles[1] = (float) (atanf(delta[1]/delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if(delta[0] >= 0.0) 
	{ 
		angles[1] += 180.0f; 
	}

	if(fFlags == 775)
		angles[0] = angles[0] - 5; 
}

bool WorldToScreen( float* vecOrigin, float* vecScreen, float *pflViewMatrix )
{
    vecScreen[ 0 ] = pflViewMatrix[ 0 ] * vecOrigin[ 0 ] + pflViewMatrix[ 1 ] * vecOrigin[ 1 ] + pflViewMatrix[ 2 ] * vecOrigin[ 2 ] + pflViewMatrix[ 3 ];
    vecScreen[ 1 ] = pflViewMatrix[ 4 ] * vecOrigin[ 0 ] + pflViewMatrix[ 5 ] * vecOrigin[ 1 ] + pflViewMatrix[ 6 ] * vecOrigin[ 2 ] + pflViewMatrix[ 7 ];
 
    float flTemp = pflViewMatrix[ 12 ] * vecOrigin[ 0 ] + pflViewMatrix[ 13 ] * vecOrigin[ 1 ] + pflViewMatrix[ 14 ] * vecOrigin[ 2 ] + pflViewMatrix[ 15 ];
 
    if( flTemp < 0.01f )
        return FALSE;
 
    float invFlTemp = 1.f / flTemp;
    vecScreen[ 0 ] *= invFlTemp;
    vecScreen[ 1 ] *= invFlTemp;
 
    static int iResolution[ 2 ] = { 0 };
    if( !iResolution[ 0 ] || !iResolution[ 1 ] )
    {
        iResolution[ 0 ] = 1920;
        iResolution[ 1 ] = 1080;
    }
 
    float x = ( float )iResolution[ 0 ] / 2.f;
    float y = ( float )iResolution[ 1 ] / 2.f;
 
    x += 0.5f * vecScreen[ 0 ] * ( float )iResolution[ 0 ] + 0.5f;
    y -= 0.5f * vecScreen[ 1 ] * ( float )iResolution[ 1 ] + 0.5f;
 
    vecScreen[ 0 ] = x;
    vecScreen[ 1 ] = y;
 
    return TRUE;
}

bool inFov(float* Origin, float Fov, float matrix[4][4])
{
    if(Fov==0)
        return TRUE;

    int CenterX = 1920/2;
    int CenterY = 1080/2;
    float screen[2];// ScreenX, ScreenY;
    if (!WorldToScreen(Origin,screen,*matrix)) return FALSE;

    return ( screen[0] <= CenterX + Fov &&
             screen[0] >= CenterX - Fov &&
             screen[1] <= CenterY + Fov &&
             screen[1] >= CenterY - Fov );
}

void normalizeAngle(float* angle) {
	for (int i=0;i<3;i++) angle[i] = (fmod((angle[i] + 180), 360)) - 180; // sanitize view angles
}

void vclamp(float* vang)
{
	if (vang[0] > 89.0f && vang[0] <= 180.0f)
		vang[0] = 88.0f;
 
	if (vang[0] > 180.0f)
		vang[0] = vang[0] - 360.0f;
 
	if (vang[0] < -89.0f)
		vang[0] = -88.0f;
 
	if (vang[1] > 180.0f)
		vang[1] = vang[1] - 360.0f;
 
	if (vang[1] < -180.0f)
		vang[1] = vang[1] + 360.0f;
 
 	vang[2] = 0;
}

#endif /* end of include guard: UTIL_H */
