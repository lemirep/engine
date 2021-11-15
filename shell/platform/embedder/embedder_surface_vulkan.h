// Copyright 2021 KDAB. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_
#define FLUTTER_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_

#include "flutter/shell/gpu/gpu_surface_vulkan.h"
#include "flutter/shell/gpu/gpu_surface_vulkan_delegate.h"
#include "flutter/shell/platform/embedder/embedder_external_view_embedder.h"
#include "flutter/shell/platform/embedder/embedder_surface.h"

namespace flutter {

class EmbedderSurfaceVulkan final : public EmbedderSurface,
                                    public GPUSurfaceVulkanDelegate {
 public:
  struct VulkanDispatchTable {
    std::function<bool(GPUVulkanTextureInfo texture)> present;
    std::function<GPUVulkanTextureInfo(const SkISize& frame_size)> acquireImage;
  };

  EmbedderSurfaceVulkan(
      const FlutterVulkanRendererConfig& config,
      const VulkanDispatchTable& dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

  ~EmbedderSurfaceVulkan() override;

 private:
  bool valid_ = false;
  std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;
  sk_sp<SkSurface> surface_;
  GrVkBackendContext main_context_;
  sk_sp<GrDirectContext> resource_context_;
  VulkanDispatchTable vk_dispatch_table_;

  // |EmbedderSurface|
  bool IsValid() const override;

  // |EmbedderSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |EmbedderSurface|
  sk_sp<GrDirectContext> CreateResourceContext() const override;

  // |GPUSurfaceVulkanDelegate|
  fml::RefPtr<vulkan::VulkanProcTable> vk() override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurfaceVulkan);
};

}  // namespace flutter

#endif  // FLUTTER_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_
