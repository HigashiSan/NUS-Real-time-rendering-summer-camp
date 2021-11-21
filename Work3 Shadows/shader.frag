#version 330 core

uniform bool RenderShadowMapMode;

in vec3 wcPosition;  
in vec3 ecPosition;  
in vec3 ecNormal;    
in vec2 texCoord;    

uniform mat4 ShadowMatrix;        

uniform vec3 LightPosition; 
uniform vec3 LightAmbient;

uniform vec3 MatlAmbient;
uniform vec3 MatlDiffuse;
uniform vec3 MatlSpecular;
uniform float MatlShininess;

uniform sampler2DShadow ShadowMap; 
uniform sampler2D ProjectorImage;  

layout (location = 0) out vec4 FragColor;

void PhongLighting(out float N_dot_L, out float R_dot_V_pow_n)
{
    vec3 viewVec = -normalize( ecPosition );
    vec3 necNormal = normalize( ecNormal );
    vec3 lightVec = normalize( LightPosition.xyz - ecPosition );
    vec3 reflectVec = reflect( -lightVec, necNormal );
    N_dot_L = max( 0.0, dot( necNormal, lightVec ) );
    float R_dot_V = max( 0.0, dot( reflectVec, viewVec ) );
    R_dot_V_pow_n = ( R_dot_V == 0.0 )? 0.0 : pow( R_dot_V, MatlShininess );
}

void DrawSceneWithProjection()
{
    vec4 lightSpacePos = ShadowMatrix * vec4(wcPosition,1);
    vec4 projCoords = lightSpacePos / lightSpacePos.w;
    vec3 color = textureProj(ProjectorImage, lightSpacePos).rgb;

    float sum = 0;
    float offset = 0.003;
    projCoords.z -= offset;
    sum += textureProjOffset(ShadowMap, projCoords, ivec2(-1,-1));
    sum += textureProjOffset(ShadowMap, projCoords, ivec2(-1,1));
    sum += textureProjOffset(ShadowMap, projCoords, ivec2(1,1));
    sum += textureProjOffset(ShadowMap, projCoords, ivec2(1,-1));
    float shadow = sum * 0.25;

    float diffuse,specular;
    PhongLighting(diffuse,specular);
    vec3 ambientColor = MatlAmbient * LightAmbient;
    vec3 diffuseColor = diffuse * MatlDiffuse * color;
    vec3 specularColor = specular * MatlSpecular * color;

    vec3 shadowFact;
    if(projCoords.x<0||projCoords.x>1||projCoords.y<0||projCoords.y>1){
        shadowFact = vec3(0,0,0);
    }
    else{
        shadowFact = vec3(shadow,shadow,shadow);
    }

    FragColor = vec4((ambientColor + shadowFact * (diffuseColor + specularColor)), 1.0f);
}

void DrawShadowMap()
{
    
}

void main()
{
    if ( RenderShadowMapMode )
        DrawShadowMap();
    else
        DrawSceneWithProjection();
    gl_FragDepth = gl_FragCoord.z;
}



