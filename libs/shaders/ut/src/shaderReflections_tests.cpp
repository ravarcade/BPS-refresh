#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "shaders/ShaderCompiler.hpp"
#include "shaders/ShaderReflections.hpp"
#include "tools/writeFile.hpp"

using namespace renderingEngine;
// fft / ifft: https://www.youtube.com/watch?v=h7apO7q16V0

namespace {

constexpr auto surceCode(const char *msg)
{
    auto begin = reinterpret_cast<const uint8_t*>(msg);
    return MemoryBuffer(begin, strlen(msg));
}

const auto imgui_vert = surceCode(R"(
#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (push_constant) uniform PushConstants {
	vec2 scale;
	vec2 translate;
} pushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	outUV = inUV;
	outColor = inColor;
	gl_Position = vec4(inPos * pushConstants.scale + pushConstants.translate, 0.0, 1.0);
}
)");

const auto imgui_frag = surceCode(R"(
#version 450

layout (binding = 0) uniform sampler2D fontSampler;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

void main() 
{
	outColor = inColor * texture(fontSampler, inUV);
}
)");

const auto shaderProgram = surceCode(R"(
#version 450

// ### Output passed to fragment shader
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outViewRay;

// ### Params UBO.  Sared with deffered.vert.glsl 
struct Light {
	vec4 position;
	vec3 color;
	float radius;
};

layout (binding = 5) uniform LUBO 
{
	Light lights[6];
	int debugSwitch;

	vec3 viewRays[3];
	vec3 eye;
	float m22;
	float m32;
	
	mat3 view2world;
} ubo; 

out gl_PerVertex
{
	vec4 gl_Position;
};

// ###
void main() 
{
	outUV = vec2(gl_VertexIndex & 2, (gl_VertexIndex << 1) & 2);                 // UV values are in range [0 .. 2], but range [1..2] is outside screen
	outViewRay = ubo.viewRays[gl_VertexIndex%3];                                 // precalculated viewRays 
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0, 1.0f);
}
)");
} // namespace

TEST(shaderReflections, compileAndParseVertexShader)
{
    ShaderCompiler sut;
    auto bin = sut.compile(shaderProgram);
    tools::writeFile("testShader.bin", bin); // for testing and manual comparation of results with glslc.exe
    ShaderReflections sr(bin, bin);
    EXPECT_TRUE(true);
}

TEST(shaderReflections, compileAndParseImguiVertexShader)
{
    ShaderCompiler sut;
    auto bin = sut.compile(imgui_vert);
    auto bin2 = sut.compile(imgui_frag);
    ShaderReflections sr(bin, bin2);
    EXPECT_TRUE(true);
}