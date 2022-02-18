//
// llvm_stacktrace.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_DETAIL_LLVM_STACKTRACE_IPP
#define GPCL_DETAIL_LLVM_STACKTRACE_IPP

#include <gpcl/detail/llvm_stacktrace.hpp>
#include <gpcl/mutex.hpp>
#include <gpcl/optional.hpp>

#include <dlfcn.h>

GPCL_GCC_SUPPRESS_WARNING_PUSH
GPCL_GCC_SUPPRESS_WARNING("-Wunused-parameter")

GPCL_CLANG_SUPPRESS_WARNING_PUSH
GPCL_CLANG_SUPPRESS_WARNING("-Wunused-parameter")

#include <llvm/DebugInfo/Symbolize/Symbolize.h>

GPCL_CLANG_SUPPRESS_WARNING_POP
GPCL_GCC_SUPPRESS_WARNING_POP

#include <unordered_map>

namespace gpcl::detail {
struct llvm_line_info : llvm::DILineInfo
{
  std::string module_name;
};

inline recursive_mutex obj_map_mutex;
inline std::unordered_map<std::string,
                          llvm::object::OwningBinary<llvm::object::ObjectFile>>
    obj_map;

optional<llvm_line_info> llvm_symbolize_code(const void *volatile address)
{
  Dl_info info{};

  if (dladdr(address, &info) == 0)
    return nullopt;

  std::uint64_t object_address = (std::uint64_t)info.dli_fbase;
  auto &&object_name = info.dli_fname;

  llvm::object::ObjectFile *obj = nullptr;

  scoped_lock lock(obj_map_mutex);
  if (auto obj_iter = obj_map.find(object_name); obj_iter != obj_map.cend())
  {
    obj = obj_iter->second.getBinary();
  }
  else
  {
    auto owning_obj = llvm::object::ObjectFile::createObjectFile(object_name);
    if (!owning_obj)
      return nullopt;

    auto &&owning_obj_ref = obj_map[object_name] = std::move(*owning_obj);

    obj = owning_obj_ref.getBinary();
  }

  std::uint64_t offset = 0;
  llvm::object::SectionRef section;

  for (auto sect : obj->sections())
  {
    if (!sect.isText())
      continue;

    const bool relative = sect.getAddress() < object_address;
    offset = (std::uint64_t)address - (relative ? object_address : 0) -
             sect.getAddress();
    if (offset < sect.getSize())
    {
      section = sect;
      break;
    }
  }
  if (!section.getObject())
    return nullopt;

  static thread_local llvm::symbolize::LLVMSymbolizer symbolizer;

  llvm::object::SectionedAddress sectioned_address = {
      section.getAddress() + offset,
      section.getIndex(),
  };

  auto line_info = symbolizer.symbolizeCode(*obj, sectioned_address);

  if (!line_info)
    return nullopt;
  llvm_line_info ret{};
  static_cast<llvm::DILineInfo &>(ret) = std::move(*line_info);
  ret.module_name = info.dli_fname;
  return ret;
}

std::string llvm_stacktrace_entry_description(const void *address)
{
  std::ostringstream ss;
  auto line_info = llvm_symbolize_code(address);
  if (!line_info)
  {
    ss << address;
    return ss.str();
  }
  ss << line_info->FunctionName;
  if (line_info->Line != 0)
    ss << " at " << line_info->FileName << '(' << line_info->Line << ')';
  else
    ss << " in " << line_info->module_name;
  return ss.str();
}

std::uint_least32_t llvm_stacktrace_entry_source_line(const void *address)
{
  auto line_info = llvm_symbolize_code(address);
  if (!line_info)
    return 0;
  return line_info->Line;
}

std::string llvm_stacktrace_entry_source_file(const void *address)
{
  auto line_info = llvm_symbolize_code(address);
  if (!line_info)
    return "";
  return line_info->FileName;
}

} // namespace gpcl::detail

#endif // GPCL_DETAIL_LLVM_STACKTRACE_IPP
