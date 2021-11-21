//============================================================================
// STUDENT NAME: Chen DongCan
// NUS User ID.: t0922729
// COMMENTS TO GRADER: 
//
//============================================================================

// FRAGMENT SHADER

#version 330 core

//============================================================================
// Pass number:
// 1 -- do image thresholding,
// 2 -- do horizontal blurring,
// 3 -- do vertical blurring,
// 4 -- do combining of original image and blurred image.
//============================================================================
uniform int PassNum;

//============================================================================
// Received from rasterizer.
//============================================================================
in vec2 texCoord;    // Fragment's texture coordinates.

//============================================================================
// Threshold value for thresholding the original image luminance.
//============================================================================
uniform float LuminanceThreshold;

//============================================================================
// Number of values in the 1D blur filter.
//============================================================================
uniform int BlurFilterWidth;  // Always an odd number.

//============================================================================
// Input texture maps.
//============================================================================
uniform sampler1D BlurFilterTex;
uniform sampler2D OriginalImageTex;
uniform sampler2D ThresholdImageTex;
uniform sampler2D HorizBlurImageTex;
uniform sampler2D VertBlurImageTex;

//============================================================================
// Outputs.
//============================================================================
layout (location = 0) out vec4 FragColor;


/////////////////////////////////////////////////////////////////////////////
// Approximates the brightness of a RGB value.
/////////////////////////////////////////////////////////////////////////////
float Luminance( vec3 color )
{
    const vec3 LuminanceWeights = vec3(0.2126, 0.7152, 0.0722);
    return dot(LuminanceWeights, color);
}


/////////////////////////////////////////////////////////////////////////////
// Threshold the original image.
/////////////////////////////////////////////////////////////////////////////
void ThresholdImage()
{
    /////////////////////////////////////////////////////////////////////////////
    // TASK:
    // * Read from the original image texture.
    // * If texture color's luminance >= LuminanceThreshold,
    //   write the texture color to output fragment,
    //   otherwise write black to output fragment.
    // * Note that the input image and the output image are different
    //   in size, so we cannot use the current fragment's coordinates
    //   as texel coordinates to read the original image texture.
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////
    // TASK: WRITE YOUR CODE HERE. //
    vec3 color = texture(OriginalImageTex, texCoord).xyz;
    float luminance = Luminance(color);
    if(luminance >= LuminanceThreshold){
        FragColor = vec4(color, 1.0f);
    }
    else{
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    /////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////
// Apply horizontal blurring to the threshold image.
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// Apply vertical blurring to the horizontally-blurred image.
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
// Add the original image to the blurred image to get the final image.
/////////////////////////////////////////////////////////////////////////////
void CombineImages()
{
    /////////////////////////////////////////////////////////////////////////////
    // TASK:
    // * Add the original image and the blurred threshold image.
    // * Note that the original image and the blurred threshold image are
    //   different in size. The original image and the output image have
    //   the same size.
    /////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////
    // TASK: WRITE YOUR CODE HERE. //
    FragColor = texture(OriginalImageTex,texCoord) + texture(VertBlurImageTex, texCoord);
    /////////////////////////////////

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
