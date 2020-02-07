/**
 * ArcGIS CityEngine SDK Geometry Encoder for Python
 *
 * Copyright (c) 2012-2020 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils.h"
#include "logging.h"

#include "prt/StringUtils.h"

#include <pybind11/pybind11.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

#ifdef _WIN32
const std::string FILE_SCHEMA = "file:/";
#else
const std::string FILE_SCHEMA = "file:";
#endif

template <typename C>
void tokenize(const std::basic_string<C> &str,
              std::vector<std::basic_string<C>> &tokens,
              const std::basic_string<C> &delimiters) {
  auto lastPos = str.find_first_not_of(delimiters, 0);
  auto pos = str.find_first_of(delimiters, lastPos);
  while (std::basic_string<C>::npos != pos ||
         std::basic_string<C>::npos != lastPos) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}

} // namespace

namespace py = pybind11;

namespace pcu {

/**
 * Helper function to convert a Python dictionary of "<key>:<value>" into a
 * prt::AttributeMap
 */
AttributeMapPtr
createAttributeMapFromPythonDict(py::dict args, prt::AttributeMapBuilder &bld) {
  for (auto a : args) {

    const std::wstring key = a.first.cast<std::wstring>();

    if (py::isinstance<py::list>(a.second.ptr())) {
      auto li = a.second.cast<py::list>();

      if (py::isinstance<py::bool_>(li[0])) {
        try {
          size_t count = li.size();
          std::unique_ptr<bool[]> v_arr(new bool[count]);

          for (size_t i = 0; i < count; i++) {
            bool item = li[i].cast<bool>();
            v_arr[i] = item;
          }

          bld.setBoolArray(key.c_str(), v_arr.get(), count);
        } catch (std::exception &e) {
          std::wcerr << L"cannot set bool array attribute " << key << ": "
                     << e.what() << std::endl;
        }
      } else if (py::isinstance<py::float_>(li[0])) {
        try {
          const size_t count = li.size();
          std::vector<double> v_arr(count);
          for (size_t i = 0; i < v_arr.size(); i++) {
            double item = li[i].cast<double>();
            v_arr[i] = item;
          }

          bld.setFloatArray(key.c_str(), v_arr.data(), v_arr.size());
        } catch (std::exception &e) {
          std::wcerr << L"cannot set float array attribute " << key << ": "
                     << e.what() << std::endl;
        }
      } else if (py::isinstance<py::int_>(li[0])) {
        try {
          const size_t count = li.size();
          std::vector<int32_t> v_arr(count);
          for (size_t i = 0; i < v_arr.size(); i++) {
            int32_t item = li[i].cast<int32_t>();
            v_arr[i] = item;
          }

          bld.setIntArray(key.c_str(), v_arr.data(), v_arr.size());
        } catch (std::exception &e) {
          std::wcerr << L"cannot set int array attribute " << key << ": "
                     << e.what() << std::endl;
        }

      } else if (py::isinstance<py::str>(li[0])) {
        const size_t count = li.size();
        std::vector<std::wstring> v_arr(count);
        for (size_t i = 0; i < v_arr.size(); i++) {
          std::wstring item = li[i].cast<std::wstring>();
          v_arr[i] = item;
        }

        const auto v_arr_ptrs =
            toPtrVec(v_arr); // setStringArray requires contiguous array
        bld.setStringArray(key.c_str(), v_arr_ptrs.data(), v_arr_ptrs.size());
      } else
        std::cout << "Unknown array type." << std::endl;
    } else {
      if (py::isinstance<py::bool_>(
              a.second.ptr())) { // check for boolean first!!
        try {
          bool val = a.second.cast<bool>();
          bld.setBool(key.c_str(), val);
        } catch (std::exception &e) {
          std::wcerr << L"cannot set bool attribute " << key << ": " << e.what()
                     << std::endl;
        }
      } else if (py::isinstance<py::float_>(a.second.ptr())) {
        try {
          double val = a.second.cast<double>();
          bld.setFloat(key.c_str(), val);
        } catch (std::exception &e) {
          std::wcerr << L"cannot set float attribute " << key << ": "
                     << e.what() << std::endl;
        }
      } else if (py::isinstance<py::int_>(a.second.ptr())) {
        try {
          int32_t val = a.second.cast<int32_t>();
          bld.setInt(key.c_str(), val);
        } catch (std::exception &e) {
          std::wcerr << L"cannot set int attribute " << key << ": " << e.what()
                     << std::endl;
        }
      } else if (py::isinstance<py::str>(a.second.ptr())) {
        std::wstring val = a.second.cast<std::wstring>();
        bld.setString(key.c_str(), val.c_str());
      } else
        std::cout << "Unknown type." << std::endl;
    }
  }
  return AttributeMapPtr{bld.createAttributeMap()};
}

/**
 * String conversion functions
 */

template <typename inC, typename outC, typename FUNC>
std::basic_string<outC> callAPI(FUNC f, const std::basic_string<inC> &s) {
  std::vector<outC> buffer(s.size());
  size_t size = buffer.size();
  f(s.c_str(), buffer.data(), &size, nullptr);
  if (size > buffer.size()) {
    buffer.resize(size);
    f(s.c_str(), buffer.data(), &size, nullptr);
  }
  return std::basic_string<outC>{buffer.data()};
}

std::string toOSNarrowFromUTF16(const std::wstring &osWString) {
  return callAPI<wchar_t, char>(prt::StringUtils::toOSNarrowFromUTF16,
                                osWString);
}

std::wstring toUTF16FromOSNarrow(const std::string &osString) {
  return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromOSNarrow,
                                osString);
}

std::wstring toUTF16FromUTF8(const std::string &utf8String) {
  return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromUTF8, utf8String);
}

std::string toUTF8FromOSNarrow(const std::string &osString) {
  std::wstring utf16String = toUTF16FromOSNarrow(osString);
  return callAPI<wchar_t, char>(prt::StringUtils::toUTF8FromUTF16, utf16String);
}

std::string percentEncode(const std::string &utf8String) {
  return callAPI<char, char>(prt::StringUtils::percentEncode, utf8String);
}

/**
 * codec info functions
 */

template <typename C, typename FUNC>
std::basic_string<C> callAPI(FUNC f, size_t initialSize) {
  std::vector<C> buffer(initialSize, ' ');

  size_t actualSize = initialSize;
  f(buffer.data(), &actualSize, nullptr);
  buffer.resize(actualSize);

  if (initialSize < actualSize)
    f(buffer.data(), &actualSize, nullptr);

  return std::basic_string<C>{buffer.data()};
}

std::string objectToXML(const prt::Object *obj) {
  auto toXMLFunc = std::bind(&prt::Object::toXML, obj, std::placeholders::_1,
                             std::placeholders::_2, std::placeholders::_3);
  return callAPI<char>(toXMLFunc, 4096);
}

RunStatus codecInfoToXML(const std::string &infoFilePath) {
  const std::wstring encIDsStr{callAPI<wchar_t>(prt::listEncoderIds, 1024)};
  const std::wstring decIDsStr{callAPI<wchar_t>(prt::listDecoderIds, 1024)};

  std::vector<std::wstring> encIDs, decIDs;
  tokenize<wchar_t>(encIDsStr, encIDs, L";");
  tokenize<wchar_t>(decIDsStr, decIDs, L";");

  try {
    std::ofstream xml(infoFilePath);
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n";

    xml << "<Codecs build=\"" << prt::getVersion()->mVersion
        << "\" buildDate=\"" << prt::getVersion()->mBuildDate
        << "\" buildConfig=\"" << prt::getVersion()->mBuildConfig << "\">\n";

    xml << "<Encoders>\n";
    for (const std::wstring &encID : encIDs) {
      prt::Status s = prt::STATUS_UNSPECIFIED_ERROR;
      const EncoderInfoPtr encInfo{prt::createEncoderInfo(encID.c_str(), &s)};
      if (s == prt::STATUS_OK && encInfo)
        xml << objectToXML(encInfo.get()) << std::endl;
      else
        LOG_ERR << L"encoder not found for ID: " << encID << std::endl;
    }
    xml << "</Encoders>\n";

    xml << "<Decoders>\n";
    for (const std::wstring &decID : decIDs) {
      prt::Status s = prt::STATUS_UNSPECIFIED_ERROR;
      const DecoderInfoPtr decInfo{prt::createDecoderInfo(decID.c_str(), &s)};
      if (s == prt::STATUS_OK && decInfo)
        xml << objectToXML(decInfo.get()) << std::endl;
      else
        LOG_ERR << L"decoder not found for ID: " << decID << std::endl;
    }
    xml << "</Decoders>\n";

    xml << "</Codecs>\n";
    xml.close();

    LOG_INF << "Dumped codecs info to " << infoFilePath;
  } catch (std::exception &e) {
    LOG_ERR << "Exception while dumping codec info: " << e.what();
    return RunStatus::FAILED;
  }

  return RunStatus::DONE;
}

URI toFileURI(const std::string &p) {
  const std::string utf8Path = toUTF8FromOSNarrow(p);
  const std::string u8PE = percentEncode(utf8Path);
  return FILE_SCHEMA + u8PE;
}

AttributeMapPtr
createValidatedOptions(const std::wstring &encID,
                       const AttributeMapPtr &unvalidatedOptions) {
  const EncoderInfoPtr encInfo{prt::createEncoderInfo(encID.c_str())};
  const prt::AttributeMap *validatedOptions = nullptr;
  encInfo->createValidatedOptionsAndStates(unvalidatedOptions.get(),
                                           &validatedOptions);
  return AttributeMapPtr(validatedOptions);
}

std::string makeGeneric(const std::string &s) {
  std::string t = s;
  std::replace(t.begin(), t.end(), '\\', '/');
  return t;
}

std::filesystem::path getLibraryPath(const void *func) {
  std::filesystem::path result;
#ifdef _WIN32
  HMODULE dllHandle = 0;
  if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)func,
                         &dllHandle)) {
    DWORD c = GetLastError();
    char msg[255];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, c, 0, msg, 255, 0);
    throw std::runtime_error(
        "error while trying to get current module handle': " +
        std::string(msg));
  }
  assert(sizeof(TCHAR) == 1);
  const size_t PATHMAXSIZE = 4096;
  TCHAR pathA[PATHMAXSIZE];
  DWORD pathSize = GetModuleFileName(dllHandle, pathA, PATHMAXSIZE);
  if (pathSize == 0 || pathSize == PATHMAXSIZE) {
    DWORD c = GetLastError();
    char msg[255];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, c, 0, msg, 255, 0);
    throw std::runtime_error(
        "error while trying to get current module path': " + std::string(msg));
  }
  result = pathA;
#else /* macosx or linux */
  Dl_info dl_info;
  if (dladdr(func, &dl_info) == 0) {
    char *error = dlerror();
    throw std::runtime_error(
        "error while trying to get current module path': " +
        std::string(error ? error : ""));
  }
  result = dl_info.dli_fname;
#endif
  return result;
}

std::filesystem::path getModuleDirectory() {
  const auto p = getLibraryPath(reinterpret_cast<const void *>(getLibraryPath));
  return p.parent_path();
}

} // namespace pcu
