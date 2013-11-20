//shader version
#version 150 core

//mode of drawing
//if is true, then use Texture
//otherwise draw gradient
uniform int useTexture;

//texture object
uniform sampler2D texture;

//retrieve this data form vertex shader
in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexIn;

out vec4 gl_FragColor;

//helper function inside fragment shader
//converts color from hsv space to rgb space
// Hue is in range 0..360
// Saturation and Lightness in range 0..1
vec3 hsv2rgb(vec3 hsvColor)
{
	//return variable
	vec3 rgbColor;
	
	//index in HSV pyramide
	int hue_index = int(floor(hsvColor.x/60.0f));

	//some helpers
	float C = hsvColor.y*hsvColor.z;
	float X = C*(1 - abs(mod(hsvColor.x/60.0f,2) - 1));
	float m = hsvColor.z-C;

	//assign color
	if (hue_index==0)
		rgbColor = vec3(C,X,0);
	else
	if (hue_index==1)
		rgbColor = vec3(X,C,0);
	else
	if (hue_index==2)
		rgbColor = vec3(0,C,X);
	else
	if (hue_index==3)
		rgbColor = vec3(0,X,C);
	else
	if (hue_index==4)
		rgbColor = vec3(X,0,C);
	else //hue_index==5
		rgbColor = vec3(C,0,X);
	
		
	return rgbColor+vec3(m,m,m);
}

//main function

//TODO: you should use VertexIn.normal value to evaluate Phong Lightning for this pixel
// 
		
void main()
{
	if (useTexture>0)
		//take color from texture using texture2D
		gl_FragColor = vec4(texture2D(texture,VertexIn.texcoord.xy).rgb,1);
	else
	{
		//draw rainbow round
		float dX = VertexIn.position.x + 0.5f;
		float dY = VertexIn.position.y - 0.5f;
		//radius to the center of square
		float R = sqrt(dX*dX + dY*dY);

		//convert from Hue-Saturation-Value to Red-Green-Blue
		vec3 rgbColor = hsv2rgb(vec3(R*360.0f,1.0f,0.5f));

		//draw pixel. 4-th component is alpha-channel. use it for transparency
		//1 is non-transparent
		//0 is transparent

		//some compilers use optimization, so use must use all attribute variables you pass to shaders
		//for example not using VertexIn.normal in this line can cause blue screen, because normals will not allocate
		//
		gl_FragColor = vec4(rgbColor.rgb,length(VertexIn.normal));//1
	}
}
