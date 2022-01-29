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
#include <llvm/DebugInfo/Symbolize/Symbolize.h>

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

optional<llvm_line_info> llvm_symbolize_code(const void *address)
{
  Dl_info info{};

  if (dladdr(address, &info) == 0)
    return nullopt;

  std::uint64_t object_address = (std::uint64_t)info.dli_fbase;
  auto &&object_name = info.dli_fname;

  auto entry_offset = (std::uint64_t)address - object_address;

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
  auto section = std::find_if(
      obj->section_begin(), obj->section_end(),
      [=](llvm::object::SectionRef section) {
        if (!section.isText())
          return false;

        if (section.getAddress() >= object_address)
          return (std::uint64_t)address >= section.getAddress() &&
                 (std::uint64_t)address <
                     section.getAddress() + section.getSize();
        return entry_offset >= section.getAddress() &&
               entry_offset < section.getAddress() + section.getSize();
      });
  if (section == obj->section_end())
    return nullopt;


  llvm::symbolize::LLVMSymbolizer symbolizer;
  auto line_info =
      symbolizer.symbolizeCode(*obj, llvm::object::SectionedAddress{
                                         (std::uint64_t)address,
                                         section->getIndex(),
                                     });

  if (!line_info)
    return nullopt;
  llvm_line_info ret{};
  static_cast<llvm::DILineInfo &>(ret) = *line_info;
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
  if (line_info->FunctionName == line_info->BadString)
    ss << address;
  else
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
