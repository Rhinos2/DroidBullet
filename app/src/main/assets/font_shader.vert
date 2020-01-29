

uniform mediump mat4 MODELVIEWPROJECTIONMATRIXF;

attribute mediump vec2 POSITIONF;
attribute lowp vec2 TEXCOORD0F;

varying lowp vec2 texcoord0;

void main( void )
{
	texcoord0 = TEXCOORD0F;
	gl_Position = MODELVIEWPROJECTIONMATRIXF * vec4( POSITIONF.x, POSITIONF.y, 0.0, 1.0 );
}

