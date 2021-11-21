#version 330 core

uniform int PassNum;

in vec2 texCoord;    
uniform float LuminanceThreshold;
uniform int BlurFilterWidth;  
uniform sampler1D BlurFilterTex;
uniform sampler2D OriginalImageTex;
uniform sampler2D ThresholdImageTex;
uniform sampler2D HorizBlurImageTex;
uniform sampler2D VertBlurImageTex;


layout (location = 0) out vec4 FragColor;

float Luminance( vec3 color )
{
    const vec3 LuminanceWeights = vec3(0.2126, 0.7152, 0.0722);
    return dot(LuminanceWeights, color);
}



void ThresholdImage()
{
   
    vec3 color = texture(OriginalImageTex, texCoord).xyz;
    float luminance = Luminance(color);
    if(luminance >= LuminanceThreshold){
        FragColor = vec4(color, 1.0f);
    }
    else{
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
}
void HorizBlurImage()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    int middle = (BlurFilterWidth - 1) / 2;

    vec4 sum = texelFetch(ThresholdImageTex, pix, 0) * texelFetch(BlurFilterTex, middle, 0).r;
    for(int i=1;i<middle;i++){
        sum += (texelFetch(ThresholdImageTex, pix + ivec2(i,0),0) * texelFetch(BlurFilterTex, middle + i, 0).r);
        sum += (texelFetch(ThresholdImageTex, pix + ivec2(-i,0),0) * texelFetch(BlurFilterTex, middle - i, 0).r);
    }

    FragColor = sum;
}

void VertBlurImage()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    int middle = (BlurFilterWidth - 1) / 2;

    vec4 sum = texelFetch(HorizBlurImageTex, pix, 0) * texelFetch(BlurFilterTex, middle, 0).r;
    for(int i=1;i<middle;i++){
        sum += (texelFetch(HorizBlurImageTex, pix + ivec2(0,i),0) * texelFetch(BlurFilterTex, middle + i, 0).r);
        sum += (texelFetch(HorizBlurImageTex, pix + ivec2(0,-i),0) * texelFetch(BlurFilterTex, middle - i, 0).r);
    }
    FragColor = sum;
}

void CombineImages()
{
    

}

void main()
{
    switch(PassNum) {
        case 1: ThresholdImage(); break;
        case 2: HorizBlurImage(); break;
        case 3: VertBlurImage(); break;
        case 4: CombineImages(); break;
    }
}
