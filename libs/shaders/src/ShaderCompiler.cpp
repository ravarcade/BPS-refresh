#include "shaders/ShaderCompiler.hpp"
#include <map>
#include "common/Logger.hpp"

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Include/glslang_c_shader_types.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/Logger.h"
#include "glslang/SPIRV/SpvTools.h"

namespace
{
using namespace glslang;
const glslang_resource_t DefaultTBuiltInResource{
    /* .max_lights = */ 32,
    /* .max_clip_planes = */ 6,
    /* .max_texture_units = */ 32,
    /* .max_texture_coords = */ 32,
    /* .max_vertex_attribs = */ 64,
    /* .max_vertex_uniform_components = */ 4096,
    /* .max_varying_floats = */ 64,
    /* .max_vertex_texture_image_units = */ 32,
    /* .max_combined_texture_image_units = */ 80,
    /* .max_texture_image_units = */ 32,
    /* .max_fragment_uniform_components = */ 4096,
    /* .max_draw_buffers = */ 32,
    /* .max_vertex_uniform_vectors = */ 128,
    /* .max_varying_vectors = */ 8,
    /* .max_fragment_uniform_vectors = */ 16,
    /* .max_vertex_output_vectors = */ 16,
    /* .max_fragment_input_vectors = */ 15,
    /* .min_program_texel_offset = */ -8,
    /* .max_program_texel_offset = */ 7,
    /* .max_clip_distances = */ 8,
    /* .max_compute_work_group_count_x = */ 65535,
    /* .max_compute_work_group_count_y = */ 65535,
    /* .max_compute_work_group_count_z = */ 65535,
    /* .max_compute_work_group_size_x = */ 1024,
    /* .max_compute_work_group_size_y = */ 1024,
    /* .max_compute_work_group_size_z = */ 64,
    /* .max_compute_texture_image_units = */ 1024,
    /* .max_compute_uniform_components = */ 16,
    /* .max_compute_image_uniforms = */ 8,
    /* .max_compute_atomic_counters = */ 8,
    /* .max_compute_atomic_counter_buffers = */ 1,
    /* .max_varying_components = */ 60,
    /* .max_vertex_output_components = */ 64,
    /* .max_geometry_input_components = */ 64,
    /* .max_geometry_output_components = */ 128,
    /* .max_fragment_input_components = */ 128,
    /* .max_image_units = */ 8,
    /* .max_combined_image_units_and_fragment_outputs = */ 8,
    /* .max_combined_shader_output_resources = */ 8,
    /* .max_image_samples = */ 0,
    /* .max_vertex_image_uniforms = */ 0,
    /* .max_tess_control_image_uniforms = */ 0,
    /* .max_tess_evaluation_image_uniforms = */ 0,
    /* .max_geometry_image_uniforms = */ 0,
    /* .max_fragment_image_uniforms = */ 8,
    /* .max_combined_image_uniforms = */ 8,
    /* .max_geometry_texture_image_units = */ 16,
    /* .max_geometry_output_vertices = */ 256,
    /* .max_geometry_total_output_components = */ 1024,
    /* .max_geometry_uniform_components = */ 1024,
    /* .max_geometry_varying_components = */ 64,
    /* .max_tess_control_input_components = */ 128,
    /* .max_tess_control_output_components = */ 128,
    /* .max_tess_control_texture_image_units = */ 16,
    /* .max_tess_control_uniform_components = */ 1024,
    /* .max_tess_control_total_output_components = */ 4096,
    /* .max_tess_evaluation_input_components = */ 128,
    /* .max_tess_evaluation_output_components = */ 128,
    /* .max_tess_evaluation_texture_image_units = */ 16,
    /* .max_tess_evaluation_uniform_components = */ 1024,
    /* .max_tess_patch_components = */ 120,
    /* .max_patch_vertices = */ 32,
    /* .max_tess_gen_level = */ 64,
    /* .max_viewports = */ 16,
    /* .max_vertex_atomic_counters = */ 0,
    /* .max_tess_control_atomic_counters = */ 0,
    /* .max_tess_evaluation_atomic_counters = */ 0,
    /* .max_geometry_atomic_counters = */ 0,
    /* .max_fragment_atomic_counters = */ 8,
    /* .max_combined_atomic_counters = */ 8,
    /* .max_atomic_counter_bindings = */ 1,
    /* .max_vertex_atomic_counter_buffers = */ 0,
    /* .max_tess_control_atomic_counter_buffers = */ 0,
    /* .max_tess_evaluation_atomic_counter_buffers = */ 0,
    /* .max_geometry_atomic_counter_buffers = */ 0,
    /* .max_fragment_atomic_counter_buffers = */ 1,
    /* .max_combined_atomic_counter_buffers = */ 1,
    /* .max_atomic_counter_buffer_size = */ 16384,
    /* .max_transform_feedback_buffers = */ 4,
    /* .max_transform_feedback_interleaved_components = */ 64,
    /* .max_cull_distances = */ 8,
    /* .max_combined_clip_and_cull_distances = */ 8,
    /* .max_samples = */ 4,
    /* .max_mesh_output_vertices_nv = */ 256,
    /* .max_mesh_output_primitives_nv = */ 512,
    /* .max_mesh_work_group_size_x_nv = */ 32,
    /* .max_mesh_work_group_size_y_nv = */ 1,
    /* .max_mesh_work_group_size_z_nv = */ 1,
    /* .max_task_work_group_size_x_nv = */ 32,
    /* .max_task_work_group_size_y_nv = */ 1,
    /* .max_task_work_group_size_z_nv = */ 1,
    /* .max_mesh_view_count_nv = */ 4,

    /* .max_mesh_output_vertices_ext = */ 256,
    /* .max_mesh_output_primitives_ext = */ 512,
    /* .max_mesh_work_group_size_x_ext = */ 32,
    /* .max_mesh_work_group_size_y_ext = */ 1,
    /* .max_mesh_work_group_size_z_ext = */ 1,
    /* .max_task_work_group_size_x_ext = */ 32,
    /* .max_task_work_group_size_y_ext = */ 1,
    /* .max_task_work_group_size_z_ext = */ 1,
    /* .max_mesh_view_count_ext = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,
    /* .limits = */
    {
        /* .non_inductive_for_loops = */ true,
        /* .while_loops = */ true,
        /* .do_while_loops = */ true,
        /* .general_uniform_indexing = */ true,
        /* .general_attribute_matrix_vector_indexing = */ true,
        /* .general_varying_indexing = */ true,
        /* .general_sampler_indexing = */ true,
        /* .general_variable_indexing = */ true,
        /* .general_constant_matrix_vector_indexing = */ true
    }};

const TBuiltInResource defaultBuiltInResources{
    /* .maxLights = */ 32,
    /* .maxClipPlanes = */ 6,
    /* .maxTextureUnits = */ 32,
    /* .maxTextureCoords = */ 32,
    /* .maxVertexAttribs = */ 64,
    /* .maxVertexUniformComponents = */ 4096,
    /* .maxVaryingFloats = */ 64,
    /* .maxVertexTextureImageUnits = */ 32,
    /* .maxCombinedTextureImageUnits = */ 80,
    /* .maxTextureImageUnits = */ 32,
    /* .maxFragmentUniformComponents = */ 4096,
    /* .maxDrawBuffers = */ 32,
    /* .maxVertexUniformVectors = */ 128,
    /* .maxVaryingVectors = */ 8,
    /* .maxFragmentUniformVectors = */ 16,
    /* .maxVertexOutputVectors = */ 16,
    /* .maxFragmentInputVectors = */ 15,
    /* .minProgramTexelOffset = */ -8,
    /* .maxProgramTexelOffset = */ 7,
    /* .maxClipDistances = */ 8,
    /* .maxComputeWorkGroupCountX = */ 65535,
    /* .maxComputeWorkGroupCountY = */ 65535,
    /* .maxComputeWorkGroupCountZ = */ 65535,
    /* .maxComputeWorkGroupSizeX = */ 1024,
    /* .maxComputeWorkGroupSizeY = */ 1024,
    /* .maxComputeWorkGroupSizeZ = */ 64,
    /* .maxComputeUniformComponents = */ 1024,
    /* .maxComputeTextureImageUnits = */ 16,
    /* .maxComputeImageUniforms = */ 8,
    /* .maxComputeAtomicCounters = */ 8,
    /* .maxComputeAtomicCounterBuffers = */ 1,
    /* .maxVaryingComponents = */ 60,
    /* .maxVertexOutputComponents = */ 64,
    /* .maxGeometryInputComponents = */ 64,
    /* .maxGeometryOutputComponents = */ 128,
    /* .maxFragmentInputComponents = */ 128,
    /* .maxImageUnits = */ 8,
    /* .maxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .maxCombinedShaderOutputResources = */ 8,
    /* .maxImageSamples = */ 0,
    /* .maxVertexImageUniforms = */ 0,
    /* .maxTessControlImageUniforms = */ 0,
    /* .maxTessEvaluationImageUniforms = */ 0,
    /* .maxGeometryImageUniforms = */ 0,
    /* .maxFragmentImageUniforms = */ 8,
    /* .maxCombinedImageUniforms = */ 8,
    /* .maxGeometryTextureImageUnits = */ 16,
    /* .maxGeometryOutputVertices = */ 256,
    /* .maxGeometryTotalOutputComponents = */ 1024,
    /* .maxGeometryUniformComponents = */ 1024,
    /* .maxGeometryVaryingComponents = */ 64,
    /* .maxTessControlInputComponents = */ 128,
    /* .maxTessControlOutputComponents = */ 128,
    /* .maxTessControlTextureImageUnits = */ 16,
    /* .maxTessControlUniformComponents = */ 1024,
    /* .maxTessControlTotalOutputComponents = */ 4096,
    /* .maxTessEvaluationInputComponents = */ 128,
    /* .maxTessEvaluationOutputComponents = */ 128,
    /* .maxTessEvaluationTextureImageUnits = */ 16,
    /* .maxTessEvaluationUniformComponents = */ 1024,
    /* .maxTessPatchComponents = */ 120,
    /* .maxPatchVertices = */ 32,
    /* .maxTessGenLevel = */ 64,
    /* .maxViewports = */ 16,
    /* .maxVertexAtomicCounters = */ 0,
    /* .maxTessControlAtomicCounters = */ 0,
    /* .maxTessEvaluationAtomicCounters = */ 0,
    /* .maxGeometryAtomicCounters = */ 0,
    /* .maxFragmentAtomicCounters = */ 8,
    /* .maxCombinedAtomicCounters = */ 8,
    /* .maxAtomicCounterBindings = */ 1,
    /* .maxVertexAtomicCounterBuffers = */ 0,
    /* .maxTessControlAtomicCounterBuffers = */ 0,
    /* .maxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .maxGeometryAtomicCounterBuffers = */ 0,
    /* .maxFragmentAtomicCounterBuffers = */ 1,
    /* .maxCombinedAtomicCounterBuffers = */ 1,
    /* .maxAtomicCounterBufferSize = */ 16384,
    /* .maxTransformFeedbackBuffers = */ 4,
    /* .maxTransformFeedbackInterleavedComponents = */ 64,
    /* .maxCullDistances = */ 8,
    /* .maxCombinedClipAndCullDistances = */ 8,
    /* .maxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxMeshOutputVerticesEXT = */ 256,
    /* .maxMeshOutputPrimitivesEXT = */ 512,
    /* .maxMeshWorkGroupSizeX_EXT = */ 32,
    /* .maxMeshWorkGroupSizeY_EXT = */ 1,
    /* .maxMeshWorkGroupSizeZ_EXT = */ 1,
    /* .maxTaskWorkGroupSizeX_EXT = */ 32,
    /* .maxTaskWorkGroupSizeY_EXT = */ 1,
    /* .maxTaskWorkGroupSizeZ_EXT = */ 1,
    /* .maxMeshViewCountEXT = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,

    /* .limits = */
    {
        /* .nonInductiveForLoops = */ true,
        /* .whileLoops = */ true,
        /* .doWhileLoops = */ true,
        /* .generalUniformIndexing = */ true,
        /* .generalAttributeMatrixVectorIndexing = */ true,
        /* .generalVaryingIndexing = */ true,
        /* .generalSamplerIndexing = */ true,
        /* .generalVariableIndexing = */ true,
        /* .generalConstantMatrixVectorIndexing = */ true
    }};

struct ShaderSettings
{
    const TBuiltInResource* builtInResources;
    int defaultVersion;
    EProfile defaultProfile;
    bool forceDefaultVersionAndProfile;
    bool forwardCompatible;
    EShMessages message;
    EShClient client;
    EShTargetClientVersion clientVersion;
    EShTargetLanguage target;
    EShTargetLanguageVersion targetVersion;
};

ShaderSettings defaults{
    &defaultBuiltInResources,
    100,
    EProfile::ENoProfile,
    false,
    false,
    EShMsgDefault,
    EShClient::EShClientVulkan,
    EShTargetClientVersion::EShTargetVulkan_1_0,
    EShTargetLanguage::EShTargetSpv,
    EShTargetLanguageVersion::EShTargetSpv_1_0};

TShader parseShader(EShLanguage stage, const char* shaderSource)
{
    TShader shader{stage};
    shader.setStrings(&shaderSource, 1);
    shader.setEnvClient(defaults.client, defaults.clientVersion);
    shader.setEnvTarget(defaults.target, defaults.targetVersion);
    if (not shader.parse(
            defaults.builtInResources, defaults.defaultVersion, defaults.forwardCompatible, defaults.message))
    {
        log_err("{}\n{}", shader.getInfoLog(), shader.getInfoDebugLog());
        throw "parseShader error";
    }
    return shader;
}

void linkProgram(TProgram& program)
{
    if (not program.link(static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules)))
    {
        log_err("{}\n{}", program.getInfoLog(), program.getInfoDebugLog());
        throw "create program error";
    }
}

std::vector<uint32_t> getSpirv(EShLanguage stage, TProgram& program)
{
    std::vector<uint32_t> spirv;
    spv::SpvBuildLogger logger;
    SpvOptions spvOptions{};
    spvOptions.validate = true;

    GlslangToSpv(*program.getIntermediate(stage), spirv, &logger, &spvOptions);

    auto spirv_messages = logger.getAllMessages();
    if (spirv_messages.size())
    {
        log_inf("{}", spirv_messages); // err?
    }
    return spirv;
}

std::vector<uint32_t> compileShaderToSPIRV_Vulkan(EShLanguage stage, const char* shaderSource)
try
{
    TShader shader = parseShader(stage, shaderSource);
    TProgram program;
    program.addShader(&shader);
    linkProgram(program);
    return getSpirv(stage, program);
}
catch (const char* errMsg)
{
    log_err("Compilation failed: {}", errMsg);
    return {};
}
} // namespace

namespace renderingEngine
{
ShaderCompiler::ShaderCompiler() : isIntialized{InitializeProcess()} {}

ShaderCompiler::~ShaderCompiler()
{
    FinalizeProcess();
}

EShLanguage getVkStage(ShaderStage stage)
{
    static std::map<ShaderStage, EShLanguage> const vulkanShader = {
        {ShaderStage::vertex, EShLanguage::EShLangVertex},
        {ShaderStage::fragment, EShLanguage::EShLangFragment},
    };
    assert(vulkanShader.find(stage) != vulkanShader.end());
    return vulkanShader.find(stage)->second;
}

std::vector<uint8_t> ShaderCompiler::compile(MemoryBuffer sourceCode, ShaderStage stage)
{
    auto spirv = compileShaderToSPIRV_Vulkan(getVkStage(stage), reinterpret_cast<const char*>(sourceCode.data()));
    return {reinterpret_cast<uint8_t*>(spirv.data()), reinterpret_cast<uint8_t*>(spirv.data() + spirv.size())};
}
} // namespace renderingEngine
