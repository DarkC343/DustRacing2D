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

#ifndef MCSHADERS_HH
#define MCSHADERS_HH

static const char * MCMasterVsh =
"#version 120\n"
""
"attribute vec3  inVertex;\n"
"attribute vec3  inNormal;\n"
"attribute vec2  inTexCoord;\n"
"attribute vec4  inColor;\n"
"uniform   vec4  scale;\n"
"uniform   vec4  color;\n"
"uniform   mat4  vp;\n"
"uniform   mat4  model;\n"
"uniform   float fade;\n"
"uniform   vec4  dd;\n"
"uniform   vec4  dc;\n"
"uniform   vec4  ac;\n"
"varying   vec2  texCoord0;\n"
"varying   vec4  vColor;\n"
""
"void main()\n"
"{"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
""
"    mat4 normalRot = mat4(mat3(model));\n"
"    float di = dot(dd, normalRot * vec4(-inNormal, 1)) * dc.a;\n"
"    vColor = inColor * color * (\n"
"        vec4(ac.rgb, 1.0) * ac.a +\n"
"        vec4(dc.rgb, 1.0) * di) * vec4(fade, fade, fade, 1.0);\n"
""
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * MCMasterFsh =
"#version 120\n"
""
"uniform sampler2D tex0;\n"
"uniform vec4      dd;\n"
"uniform vec4      dc;\n"
"varying vec2      texCoord0;\n"
"varying vec4      vColor;\n"
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
"        gl_FragColor = vColor * texColor;\n"
"    }\n"
"}\n";

static const char * MCMasterFshSpecular =
"#version 120\n"
""
"uniform sampler2D tex0;\n"
"uniform float     fade;\n"
"uniform vec4      dd;\n"
"uniform vec4      dc;\n"
"varying vec2      texCoord0;\n"
"varying vec4      vColor;\n"
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
"        vec4 sc  = vec4(1.0, 1.0, 1.0, 1.0);\n"
"        vec4 eye = vec4(840, 500, 100, 0.0);\n"
"        vec4 pos = vec4(gl_FragCoord.xy, 0.0, 0.0);\n"
"        vec4 V   = normalize(eye - pos);\n"
"        vec4 L   = -dd;\n"
"        vec4 N   = vec4(0.0, 0.0, 1.0, 0.0);\n"
""
"        float si = 0.0;\n"
"        if (dot(N, L) > 0.0)\n"
"        {\n"
"            vec4 R = (N * dot(N, L) * 2.0) - L;\n"
"    	     si = max(0.0, pow(dot(R, V), 10));\n"
"        }\n"
""
"        gl_FragColor = vColor * texColor + sc * si;\n"
"    }\n"
"}\n";

static const char * MCMasterShadowVsh =
"#version 120\n"
""
"attribute vec3 inVertex;\n"
"attribute vec3 inNormal;\n"
"attribute vec2 inTexCoord;\n"
"attribute vec4 inColor;\n"
"uniform   vec4 scale;\n"
"uniform   vec4 color;\n"
"uniform   mat4 vp;\n"
"uniform   mat4 model;\n"
"varying   vec2 texCoord0;\n"
"varying   vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
"    vColor      = inColor * color;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * MCMasterShadowFsh =
"#version 120\n"
""
"uniform sampler2D texture;\n"
"varying vec2      texCoord0;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color = texture2D(texture, texCoord0);\n"
"    if (color.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    }\n"
"}\n";

#endif // MCSHADERS_HH
