#version 330 core

uniform bool RenderShadowMapMode;

layout (location = 0) in vec4 vPosition;  
layout (location = 1) in vec3 vNormal;    
layout (location = 2) in vec2 vTexCoord; 

uniform mat4 ModelMatrix;         
uniform mat4 ModelViewMatrix;     
uniform mat4 ModelViewProjMatrix; 
uniform mat3 NormalMatrix;        
                                 
out vec3 wcPosition;   
out vec3 ecPosition;   
out vec3 ecNormal;    
out vec2 texCoord;     


void main()
{
    if ( !RenderShadowMapMode ) {
        wcPosition = vec3( ModelMatrix * vPosition );
        ecPosition = vec3( ModelViewMatrix * vPosition );
        ecNormal = normalize( NormalMatrix * vNormal );
        texCoord = vTexCoord;
    }
    gl_Position = ModelViewProjMatrix * vPosition;
}
