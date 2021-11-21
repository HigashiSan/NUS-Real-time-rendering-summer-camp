

#version 330 core

//============================================================================
// Received from rasterizer.
//============================================================================
in vec3 ecPosition;   // Fragment's 3D position in eye space.
in vec3 ecNormal;     // Fragment's normal vector in eye space.
in vec3 v2fTexCoord;  // Fragment's texture coordinates. It is 3D when it is
                      //   used as texture coordinates to a cubemap.
in vec2 texCoord;
in vec4 viewPos;

//============================================================================
// Indicates which object is being rendered.
// 0 -- draw skybox, 1 -- draw brick cube, 2 -- draw wooden cube.
//============================================================================
uniform int WhichObj;

//============================================================================
// View and projection matrices, etc.
//============================================================================
uniform mat4 ViewMatrix;          // View transformation matrix.
uniform mat4 ModelViewMatrix;     // ModelView matrix.
uniform mat4 ModelViewProjMatrix; // ModelView matrix * Projection matrix.
uniform mat3 NormalMatrix;        // For transforming object-space direction
                                  //   vector to eye space.

//============================================================================
// Light info.
//============================================================================
uniform vec4 LightPosition; // Given in eye space. Can be directional.
uniform vec4 LightAmbient;
uniform vec4 LightDiffuse;
uniform vec4 LightSpecular;

//============================================================================
// Material info.
//============================================================================
// Specular material of the brick surface.
const vec3 BrickSpecularMaterial = vec3(1.0, 1.0, 1.0);

// Material shininess of specular reflection on the brick surface.
const float BrickShininess = 128.0;

// Specular material of the wood surface.
const vec3 WoodSpecularMaterial = vec3(1.0, 1.0, 1.0);

// Material shininess of specular reflection on the wood surface.
const float WoodShininess = 128.0;


uniform samplerCube EnvMap;
uniform sampler2D BrickDiffuseMap;
uniform sampler2D BrickNormalMap;
uniform sampler2D WoodDiffuseMap;
const float MirrorTileDensity = 2.0;  // (0.0, inf)
const float MirrorRadius = 0.4;  // (0.0, 0.5]
const float DeltaNormal_Z_Scale = 1.0 / 5.0;
layout (location = 0) out vec4 FragColor;

void drawSkybox()
{
    FragColor = texture(EnvMap, v2fTexCoord);
}

void compute_tangent_vectors( vec3 N, vec3 p, vec2 uv, out vec3 T, out vec3 B )
{
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
    
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    T = normalize( dp2perp * duv1.x + dp1perp * duv2.x );  // Tangent vector
    B = normalize( dp2perp * duv1.y + dp1perp * duv2.y );  // Binormal vector
}

/////////////////////////////////////////////////////////////////////////////
// Compute fragment color on brick cube.
/////////////////////////////////////////////////////////////////////////////
void drawBrickCube()
{
    if (gl_FrontFacing) {
        vec3 viewVec = -normalize(ecPosition);
        vec3 necNormal = normalize(ecNormal);

        vec3 lightVec;
        if (LightPosition.w == 0.0 )
            lightVec = normalize(LightPosition.xyz);
        else
            lightVec = normalize(LightPosition.xyz - ecPosition);

        

        
        vec3 tangent,bitangent;
        compute_tangent_vectors(necNormal,ecPosition,texCoord,tangent,bitangent);
        mat3 TBN = mat3(tangent,bitangent,necNormal);

        vec3 normal = texture(BrickNormalMap, texCoord).rgb;
        
        normal = normalize(normal * 2.0 - 1.0);
        normal.z = normal.z * DeltaNormal_Z_Scale;
        normal = normalize(TBN * normal);

        vec3 color = texture(BrickDiffuseMap, texCoord).rgb;
        vec3 ambient = color * LightAmbient.rgb;

        vec3 diffuse = color * LightDiffuse.rgb * max(dot(lightVec,normal),0);

        vec3 reflectDir = reflect(-lightVec,normal);
        vec3 halfwayDir = normalize(lightVec + viewVec);
        vec3 specular = LightSpecular.rgb * BrickSpecularMaterial * pow(max(dot(normal,halfwayDir),0),BrickShininess) ;

        FragColor = vec4(ambient + diffuse + specular , 1.0f);
    }
    else discard;
}


void drawWoodenCube()
{
    if (gl_FrontFacing) {
        vec3 viewVec = -normalize(ecPosition);
        vec3 necNormal = normalize(ecNormal);

        vec3 lightVec;
        if (LightPosition.w == 0.0 )
            lightVec = normalize(LightPosition.xyz);
        else
            lightVec = normalize(LightPosition.xyz - ecPosition);

       
        vec3 color = texture(WoodDiffuseMap,texCoord).rgb;
        vec2 c = MirrorTileDensity * texCoord;
        vec2 p = fract(c) - vec2(0.5);
        float sqrDist = p.x * p.x + p.y * p.y;

        if(sqrDist > MirrorRadius * MirrorRadius){
            vec3 ambient = LightAmbient.rgb * color;
            vec3 diffuse = LightDiffuse.rgb * color * max(dot(lightVec,necNormal),0.0);

            vec3 viewDir = normalize(viewPos.xyz - ecPosition);
            vec3 reflectDir = reflect(-lightVec,necNormal);
            vec3 specular = WoodSpecularMaterial * LightSpecular.rgb * pow(max(dot(viewVec,reflectDir),0),WoodShininess);

            FragColor = vec4(ambient + diffuse + specular, 1.0f);
        }
        else{
            vec3 tangent,bitangent;
            compute_tangent_vectors(necNormal,ecPosition,texCoord,tangent,bitangent);
            mat3 TBN = mat3(tangent,bitangent,necNormal);

            p /= MirrorRadius;
            vec3 tanPerturbedNormal = vec3(p, sqrt(1 - p.x * p.x - p.y * p.y));
            vec3 ecPerturbedNormal = TBN * tanPerturbedNormal;

            vec3 reflectDir = reflect(viewVec,ecPerturbedNormal);
            reflectDir = (transpose(ViewMatrix) * vec4(reflectDir.xyz,1.0)).xyz;

            vec4 mirrorColor = texture(EnvMap, -reflectDir);

            FragColor = mirrorColor;
        }

        ///////////////////////////////////

    }
    else discard;
}



void main()
{
    switch(WhichObj) {
        case 0: drawSkybox(); break;
        case 1: drawBrickCube(); break;
        case 2: drawWoodenCube(); break;
    }
}
