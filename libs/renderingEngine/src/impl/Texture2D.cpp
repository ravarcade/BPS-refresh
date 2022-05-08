#include "Texture2D.hpp"
#include "Context.hpp"
#include "DescriptorSetManager.hpp"
#include "common/Image.hpp"

namespace
{
using namespace common;
} // namespace

namespace renderingEngine
{
Texture2D::Texture2D(Context& context, common::Image& image) : Texture{context}
{
    loadTexture(image);
}

Texture2D::~Texture2D() {}

void Texture2D::loadTexture(common::Image& img)
{
    context.createImage(*this, img);
    context.copyToStagingBuffer(img);

    context.transitionImageLayout(
        image,
        0, // baseMipmap
        img.num_mipmaps,
        0, // baseLayer
        img.num_faces * img.num_layers,
        0, // srcAccessFlags
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT);

    context.copyBufferToImage(image, img);
    context.executeSingleTimeCommands(context.commandBuffer);

    if (mipLevels != img.num_mipmaps)
    {
        context.transitionImageLayout(
            image,
            0,
            img.num_mipmaps,
            0,
            img.num_faces * img.num_layers,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);

        for (uint32_t mip = img.num_mipmaps; mip < mipLevels; mip++)
        {
            context.transitionImageLayout(
                image,
                mip,
                1,
                0,
                img.num_faces * img.num_layers,
                0,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT);

            context.blitImage(image, mip - 1, mip, img);

            context.transitionImageLayout(
                image,
                mip,
                1,
                0,
                img.num_faces * img.num_layers,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT);
        }
        context.transitionImageLayout(
            image,
            0,
            mipLevels,
            0,
            img.num_faces * img.num_layers,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
    else
    {
        context.transitionImageLayout(
            image,
            0,
            mipLevels,
            0,
            img.num_faces * img.num_layers,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }
    // vkt.Execute();
    context.executeSingleTimeCommands(context.commandBuffer);

    // === create sampler
    // ============================================================================================================

    sampler = context.createSampler(mipLevels, true);

    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // context.descriptorSetManager->markDescriptorSetsInvalid(&descriptor); // rav: problem
    // TRACE("Created texture: " << resImg.GetName() << "\n");
}
} // namespace renderingEngine
