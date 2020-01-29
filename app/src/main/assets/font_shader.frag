

uniform sampler2D DIFFUSEF;
uniform lowp vec4 COLORF;

varying lowp vec2 texcoord0;

void main( void )
 {
	lowp vec4 color = texture2D( DIFFUSEF, texcoord0 );
	color.x = COLORF.x;
	color.y = COLORF.y;
	color.z = COLORF.z;
	color.w *= COLORF.w;

	gl_FragColor = color;
}
