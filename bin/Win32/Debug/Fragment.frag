//shader version
#version 150 core

//mode of drawing
//if is true, then use Texture
//otherwise draw gradient
uniform int useTexture;
uniform int perceivesLight;

//texture object
uniform sampler2D textureSampler;

//retrieve this data form vertex shader
in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexIn;

out vec4 fragColor;

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

void main()
{
	float alpha = 1; 
	if (perceivesLight == 3)
		alpha = 0.1;

	if (useTexture>0)
		fragColor = vec4(texture(textureSampler,VertexIn.texcoord.xy).rgb, alpha);
	
	if (perceivesLight == 1 || perceivesLight == 3)
	{
		vec4 Iamb = vec4(-0.3, -0.3, -0.3, -0.07);

		vec4 Idiff = vec4(0.8, 0.8, 0.8, max(dot(VertexIn.normal, normalize(-VertexIn.position)), 0.0) / 2) * max(dot(VertexIn.normal, normalize(-VertexIn.position)), 0.0); //diffuse
		Idiff = clamp(Idiff, 0.0, 1.0);
  
		//fragColor = vec4(VertexIn.normal, 1.0);
		fragColor += Iamb + Idiff;
	}
	else
	if (perceivesLight == 0)
	{
		fragColor += vec4(0.3, 0.3, 0.3, 0.5);
	}
	
}
