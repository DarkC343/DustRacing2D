// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef SHADERS_H
#define SHADERS_H

static const char * carVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 pos;\n"
"uniform vec4 scale;\n"
"uniform vec2 angle;\n"
"uniform vec4 color;\n"
"uniform mat4 mvp;\n"
"uniform float fade;\n"
"uniform vec4 diffuseLightDir;\n"
"uniform vec4 diffuseLightColor;\n"
"uniform vec4 ambientLightColor;\n"
"out vec4 diffuseLightDirRotated;\n"
"out vec2 texCoord0;\n"
"out vec2 texCoord1;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    float sin1 = angle.x;\n"
"    float cos1 = angle.y;\n"
""
"    mat4 transformation = mat4(\n"
"        cos1,  sin1,  0.0,   0.0,\n"
"        -sin1, cos1,  0.0,   0.0,\n"
"        0.0,   0.0,   1.0,   0.0,\n"
"        pos.x, pos.y, pos.z, 1.0);\n"
""
"    gl_Position = mvp * transformation * (vec4(inVertex, 1) * scale);\n"
""
"    float diffuseLightIntensity = dot(normalize(diffuseLightDir), vec4(-inNormal, 1)) * diffuseLightColor.a * 0.5;\n"
"    vColor = inColor * color * (\n"
"        vec4(ambientLightColor.rgb, 1.0) * ambientLightColor.a +\n"
"        vec4(diffuseLightColor.rgb, 1.0) * diffuseLightIntensity) * fade;\n"
""
"    // We need this for normal mapping in the fragment shader.\n"
"    mat4 lightTransformation = mat4(\n"
"        cos1,  sin1,  0.0,   0.0,\n"
"        -sin1, cos1,  0.0,   0.0,\n"
"        0.0,   0.0,   1.0,   0.0,\n"
"        0.0,   0.0,   0.0,   1.0);\n"
"    diffuseLightDirRotated = vec4(-diffuseLightDir.xyz, 1) * lightTransformation;\n"
""
"    texCoord0 = inTexCoord;\n"
""
"    mat4 skyReflectionOrientation = mat4(\n"
"        cos1, -sin1, 0.0, 0.0,\n"
"        sin1,  cos1, 0.0, 0.0,\n"
"        0.0,    0.0, 1.0, 0.0,\n"
"        0.0,    0.0, 0.0, 1.0);\n"
"    texCoord1 = (vec4(inTexCoord, 0.0, 0.0) * skyReflectionOrientation).xy;\n"
"}\n";

static const char * carFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"uniform vec4 diffuseLightDir;\n"
"uniform vec4 diffuseLightColor;\n"
"in vec4 diffuseLightDirRotated;\n"
"in vec2 texCoord0;\n"
"in vec2 texCoord1;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
"    vec4 sky      = texture2D(tex1, texCoord1);\n"
"    vec4 map      = texture2D(tex2, texCoord0);\n"
""
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"         // Sky reflection\n"
"         const float keyThreshold = 0.75;\n"
"         if (texColor.r < keyThreshold && texColor.g > keyThreshold && texColor.b < keyThreshold)\n"
"         {\n"
"             float intensity = 0.75;\n"
"             fragColor = sky * intensity * texColor.g * vColor;\n"
"         }\n"
"         else\n"
"         {\n"
"             fragColor = (texColor * vColor).rgba * dot(normalize(diffuseLightDirRotated), ((map - vec4(0.5, 0.5, 0.5, 0)) * 2));\n"
"         }\n"
"    }\n"
"}\n";

static const char * fboVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec2 inTexCoord;\n"
"uniform vec4 pos;\n"
"uniform vec4 scale;\n"
"uniform mat4 mvp;\n"
"out vec2 texCoord0;\n"
""
"void main()\n"
"{\n"
"    mat4 translation = mat4(\n"
"        1.0,   0.0,   0.0,   0.0,\n"
"        0.0,   1.0,   0.0,   0.0,\n"
"        0.0,   0.0,   1.0,   0.0,\n"
"        pos.x, pos.y, pos.z, 1.0);\n"
""
"    gl_Position = mvp * translation * (vec4(inVertex, 1) * scale);\n"
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * fboFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    fragColor = texture2D(tex0, texCoord0);\n"
"}\n";

static const char * menuVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 pos;\n"
"uniform vec4 scale;\n"
"uniform vec2 angle;\n"
"uniform vec4 color;\n"
"uniform mat4 mvp;\n"
"uniform float fade;\n"
"out vec2 texCoord0;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    float sin1 = angle.x;\n"
"    float cos1 = angle.y;\n"
""
"    mat4 translation = mat4(\n"
"        cos1,  sin1,  0.0,   0.0,\n"
"        -sin1, cos1,  0.0,   0.0,\n"
"        0.0,   0.0,   1.0,   0.0,\n"
"        pos.x, pos.y, pos.z, 1.0);\n"
""
"    gl_Position = mvp * translation * (vec4(inVertex, 1) * scale);\n"
"    vColor      = inColor * color * fade;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * menuFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = vColor * texColor;\n"
"    }\n"
"}\n";

static const char * particleFsh =
"#version 130\n"
""
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    fragColor = vColor;\n"
"}\n";

static const char * pointParticleVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec4 inColor;\n"
"uniform mat4 mvp;\n"
"uniform float fade;\n"
"uniform float pointSize;\n"
"out mat2 rotationMatrix;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = mvp * vec4(inVertex, 1);\n"
"    gl_PointSize = pointSize;\n"
"    vColor = inColor * fade;\n"
"    // Rotation matrix to rotate a point sprite (sin and cos delivered as normals)\n"
"    rotationMatrix = mat2(inNormal.x, inNormal.y, -inNormal.y, inNormal.x);\n"
"}\n";

static const char * pointParticleFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    fragColor = texture2D(tex0, gl_PointCoord) * vColor;\n"
"}\n";

static const char * pointParticleDiscardFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec4 vColor;\n"
"in mat2 rotationMatrix;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, rotationMatrix * gl_PointCoord) * vColor * 0.5;\n"
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = texColor;\n"
"    }\n"
"}\n";

static const char * textVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 pos;\n"
"uniform vec4 scale;\n"
"uniform vec4 color;\n"
"uniform mat4 mvp;\n"
"uniform float fade;\n"
"out vec2 texCoord0;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = mvp * (vec4(inVertex, 1) * scale + pos);\n"
"    vColor      = inColor * color * fade;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * textFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
""
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = vColor * texColor;\n"
"    }\n"
"}\n";

static const char * textShadowFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color = texture2D(tex0, texCoord0);\n"
"    if (color.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    }\n"
"}\n";

static const char * tileVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 pos;\n"
"uniform vec4 scale;\n"
"uniform vec2 angle;\n"
"uniform mat4 mvp;\n"
"uniform float fade;\n"
"uniform vec4 diffuseLightDir;\n"
"uniform vec4 diffuseLightColor;\n"
"uniform vec4 ambientLightColor;\n"
"out vec2 texCoord0;\n"
"out vec2 texCoord1;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    float sin1 = angle.x;\n"
"    float cos1 = angle.y;\n"
""
"    mat4 transformation = mat4(\n"
"        cos1,  sin1,  0.0,   0.0,\n"
"        -sin1, cos1,  0.0,   0.0,\n"
"        0.0,   0.0,   1.0,   0.0,\n"
"        pos.x, pos.y, pos.z, 1.0);\n"
""
"    gl_Position = mvp * transformation * vec4(inVertex, 1);\n"
"    float diffuseLightIntensity = dot(normalize(diffuseLightDir), vec4(-inNormal, 1)) * diffuseLightColor.a;\n"
"    vColor = inColor * (\n"
"        vec4(ambientLightColor.rgb, 1.0) * ambientLightColor.a +\n"
"        vec4(diffuseLightColor.rgb, 1.0) * diffuseLightIntensity) * fade;\n"
""
"    texCoord0 = inTexCoord;\n"
"    texCoord1 = inTexCoord;\n"
"}\n";

static const char * tile2dFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = texColor * vColor;\n"
"    }\n"
"}\n";

static const char * tile3dFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"
"uniform sampler2D tex2;\n"
"in vec2 texCoord0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color0 = texture2D(tex0, texCoord0);\n"
"    vec4 color1 = texture2D(tex1, texCoord0);\n"
"    vec4 color2 = texture2D(tex2, texCoord0);\n"
""
"    if (color0.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        if (color0.r == 0.0 && color0.b == 0.0)\n"
"        {\n"
"            color0 = mix(color1, color2, color0.g);\n"
"        }\n"
"        else if (color0.g == 0.0 && color0.b == 0.0)\n"
"        {\n"
"            color0 = color1;\n"
"        }\n"
""
"        fragColor = color0 * vColor;\n"
"    }\n"
"}\n";

#endif // SHADERS_H
