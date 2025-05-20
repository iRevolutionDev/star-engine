#pragma once

#include <glsl/f_simple.sc.bin.h>
#include <essl/f_simple.sc.bin.h>
#include <spirv/f_simple.sc.bin.h>

#include <glsl/v_simple.sc.bin.h>
#include <essl/v_simple.sc.bin.h>
#include <spirv/v_simple.sc.bin.h>

#if defined(_WIN32)
#include <dx10/f_simple.sc.bin.h>
#include <dx10/v_simple.sc.bin.h>
#include <dx11/f_simple.sc.bin.h>
#include <dx11/v_simple.sc.bin.h>
#endif
#if __APPLE__
 #include <mtl/f_simple.sc.bin.h>
 #include <mtl/v_simple.sc.bin.h>

 #include <mtl/f_imgui.sc.bin.h>
 #include <mtl/v_imgui.sc.bin.h>
#endif


const bgfx::EmbeddedShader k_simple_vs = BGFX_EMBEDDED_SHADER(v_simple);
const bgfx::EmbeddedShader k_simple_fs = BGFX_EMBEDDED_SHADER(f_simple);

