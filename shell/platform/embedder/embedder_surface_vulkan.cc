// Copyright 2021 KDAB. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/embedder/embedder_surface_vulkan.h"
#include "flutter/fml/logging.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

namespace flutter {

EmbedderSurfaceVulkan::EmbedderSurfaceVulkan(
    const FlutterVulkanRendererConfig& config,
    const VulkanDispatchTable& dispatch_table,
    std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder)
    : GPUSurfaceVulkanDelegate(),
      external_view_embedder_(external_view_embedder),
      vk_dispatch_table_(dispatch_table) {
  // We pass in the config rather than having to pass all of these individually
  main_context_.fInstance = (VkInstance)config.instance;
  main_context_.fPhysicalDevice = (VkPhysicalDevice)config.physicalDevice;
  main_context_.fDevice = (VkDevice)config.logicalDevice;
  main_context_.fQueue = (VkQueue)config.graphicsQueue;
  main_context_.fGraphicsQueueIndex = config.graphicsQueueIndex;
  main_context_.fOwnsInstanceAndDevice = false;

  resource_context_ = GrDirectContext::MakeVulkan(main_context_);
  valid_ = true;
}

EmbedderSurfaceVulkan::~EmbedderSurfaceVulkan() = default;

bool EmbedderSurfaceVulkan::IsValid() const {
  return valid_;
}

std::unique_ptr<Surface> EmbedderSurfaceVulkan::CreateGPUSurface() {
  if (!IsValid()) {
    return nullptr;
  }

  // Note: for now we probably went to disable the render_to_surface
  // so that GPUSurfaceVulkan doesn't create a VulkanWindow with its swapchain
  // Yet that would currently mean that an empty SurfaceFrame is returned by
  // the GPUSurfaceVulkan
  const bool render_to_surface = !external_view_embedder_;
  std::unique_ptr<vulkan::VulkanNativeSurface> native_surface = {};

  // TO DO: We probably will need to modify GPUSurfaceVulkan to pass in
  // so callbacks to handle acquiring a new image, recording of the work,
  // submitting/presenting the queue
  auto surface = std::make_unique<GPUSurfaceVulkan>(
      resource_context_, this, std::move(native_surface), render_to_surface);

  if (!surface->IsValid()) {
    return nullptr;
  }

  return surface;
}

sk_sp<GrDirectContext> EmbedderSurfaceVulkan::CreateResourceContext() const {
  return resource_context_;
}

fml::RefPtr<vulkan::VulkanProcTable> EmbedderSurfaceVulkan::vk() {
  return {};
}

}  // namespace flutter
