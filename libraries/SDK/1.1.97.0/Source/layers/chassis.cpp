
// This file is ***GENERATED***.  Do Not Edit.
// See layer_chassis_generator.py for modifications.

/* Copyright (c) 2015-2019 The Khronos Group Inc.
 * Copyright (c) 2015-2019 Valve Corporation
 * Copyright (c) 2015-2019 LunarG, Inc.
 * Copyright (c) 2015-2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */


#include <string.h>
#include <mutex>

#define VALIDATION_ERROR_MAP_IMPL

#include "chassis.h"
#include "layer_chassis_dispatch.h"

std::unordered_map<void*, ValidationObject*> layer_data_map;

// Global unique object identifier.  All increments must be guarded by a lock.
uint64_t global_unique_id = 1;
// Map uniqueID to actual object handle
std::unordered_map<uint64_t, uint64_t> unique_id_mapping;

// TODO: This variable controls handle wrapping -- in the future it should be hooked
//       up to the new VALIDATION_FEATURES extension. Temporarily, control with a compile-time flag.
#if defined(LAYER_CHASSIS_CAN_WRAP_HANDLES)
bool wrap_handles = true;
#else
const bool wrap_handles = false;
#endif

// Include child object (layer) definitions
#if BUILD_OBJECT_TRACKER
#include "object_lifetime_validation.h"
#define OBJECT_LAYER_NAME "VK_LAYER_LUNARG_object_tracker"
#elif BUILD_THREAD_SAFETY
#include "thread_safety.h"
#define OBJECT_LAYER_NAME "VK_LAYER_GOOGLE_threading"
#elif BUILD_PARAMETER_VALIDATION
#include "stateless_validation.h"
#define OBJECT_LAYER_NAME "VK_LAYER_LUNARG_parameter_validation"
#elif BUILD_CORE_VALIDATION
#define OBJECT_LAYER_NAME "VK_LAYER_LUNARG_core_validation"
#else
#define OBJECT_LAYER_NAME "VK_LAYER_GOOGLE_unique_objects"
#endif

namespace vulkan_layer_chassis {

using std::unordered_map;

static const VkLayerProperties global_layer = {
    OBJECT_LAYER_NAME, VK_LAYER_API_VERSION, 1, "LunarG validation Layer",
};

static const VkExtensionProperties instance_extensions[] = {{VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_SPEC_VERSION}};

extern const std::unordered_map<std::string, void*> name_to_funcptr_map;


// Manually written functions

// Check enabled instance extensions against supported instance extension whitelist
static void InstanceExtensionWhitelist(ValidationObject *layer_data, const VkInstanceCreateInfo *pCreateInfo, VkInstance instance) {
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        // Check for recognized instance extensions
        if (!white_list(pCreateInfo->ppEnabledExtensionNames[i], kInstanceExtensionNames)) {
            log_msg(layer_data->report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, 0,
                    kVUIDUndefined,
                    "Instance Extension %s is not supported by this layer.  Using this extension may adversely affect validation "
                    "results and/or produce undefined behavior.",
                    pCreateInfo->ppEnabledExtensionNames[i]);
        }
    }
}

// Check enabled device extensions against supported device extension whitelist
static void DeviceExtensionWhitelist(ValidationObject *layer_data, const VkDeviceCreateInfo *pCreateInfo, VkDevice device) {
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        // Check for recognized device extensions
        if (!white_list(pCreateInfo->ppEnabledExtensionNames[i], kDeviceExtensionNames)) {
            log_msg(layer_data->report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, 0,
                    kVUIDUndefined,
                    "Device Extension %s is not supported by this layer.  Using this extension may adversely affect validation "
                    "results and/or produce undefined behavior.",
                    pCreateInfo->ppEnabledExtensionNames[i]);
        }
    }
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char *funcName) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    if (!ApiParentExtensionEnabled(funcName, layer_data->device_extensions.device_extension_set)) {
        return nullptr;
    }
    const auto &item = name_to_funcptr_map.find(funcName);
    if (item != name_to_funcptr_map.end()) {
        return reinterpret_cast<PFN_vkVoidFunction>(item->second);
    }
    auto &table = layer_data->device_dispatch_table;
    if (!table.GetDeviceProcAddr) return nullptr;
    return table.GetDeviceProcAddr(device, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char *funcName) {
    const auto &item = name_to_funcptr_map.find(funcName);
    if (item != name_to_funcptr_map.end()) {
        return reinterpret_cast<PFN_vkVoidFunction>(item->second);
    }
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    auto &table = layer_data->instance_dispatch_table;
    if (!table.GetInstanceProcAddr) return nullptr;
    return table.GetInstanceProcAddr(instance, funcName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetPhysicalDeviceProcAddr(VkInstance instance, const char *funcName) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    auto &table = layer_data->instance_dispatch_table;
    if (!table.GetPhysicalDeviceProcAddr) return nullptr;
    return table.GetPhysicalDeviceProcAddr(instance, funcName);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    return util_GetLayerProperties(1, &global_layer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount,
                                                              VkLayerProperties *pProperties) {
    return util_GetLayerProperties(1, &global_layer, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties) {
    if (pLayerName && !strcmp(pLayerName, global_layer.layerName))
        return util_GetExtensionProperties(1, instance_extensions, pCount, pProperties);

    return VK_ERROR_LAYER_NOT_PRESENT;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName,
                                                                  uint32_t *pCount, VkExtensionProperties *pProperties) {
    if (pLayerName && !strcmp(pLayerName, global_layer.layerName)) return util_GetExtensionProperties(0, NULL, pCount, pProperties);
    assert(physicalDevice);
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    return layer_data->instance_dispatch_table.EnumerateDeviceExtensionProperties(physicalDevice, NULL, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                              VkInstance *pInstance) {
    VkLayerInstanceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) return VK_ERROR_INITIALIZATION_FAILED;
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    uint32_t specified_version = (pCreateInfo->pApplicationInfo ? pCreateInfo->pApplicationInfo->apiVersion : VK_API_VERSION_1_0);
    uint32_t api_version = (specified_version < VK_API_VERSION_1_1) ? VK_API_VERSION_1_0 : VK_API_VERSION_1_1;


    // Create temporary dispatch vector for pre-calls until instance is created
    std::vector<ValidationObject*> local_object_dispatch;
#if BUILD_OBJECT_TRACKER
    auto object_tracker = new ObjectLifetimes;
    local_object_dispatch.emplace_back(object_tracker);
    object_tracker->container_type = LayerObjectTypeObjectTracker;
    object_tracker->api_version = api_version;
#elif BUILD_THREAD_SAFETY
    auto thread_checker = new ThreadSafety;
    local_object_dispatch.emplace_back(thread_checker);
    thread_checker->container_type = LayerObjectTypeThreading;
    thread_checker->api_version = api_version;
#elif BUILD_PARAMETER_VALIDATION
    auto parameter_validation = new StatelessValidation;
    local_object_dispatch.emplace_back(parameter_validation);
    parameter_validation->container_type = LayerObjectTypeParameterValidation;
    parameter_validation->api_version = api_version;
#endif


    // Init dispatch array and call registration functions
    for (auto intercept : local_object_dispatch) {
        intercept->PreCallValidateCreateInstance(pCreateInfo, pAllocator, pInstance);
    }
    for (auto intercept : local_object_dispatch) {
        intercept->PreCallRecordCreateInstance(pCreateInfo, pAllocator, pInstance);
    }

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) return result;

    auto framework = GetLayerDataPtr(get_dispatch_key(*pInstance), layer_data_map);

    framework->object_dispatch = local_object_dispatch;

    framework->instance = *pInstance;
    layer_init_instance_dispatch_table(*pInstance, &framework->instance_dispatch_table, fpGetInstanceProcAddr);
    framework->report_data = debug_utils_create_instance(&framework->instance_dispatch_table, *pInstance, pCreateInfo->enabledExtensionCount,
                                                         pCreateInfo->ppEnabledExtensionNames);
    framework->api_version = api_version;
    framework->instance_extensions.InitFromInstanceCreateInfo(specified_version, pCreateInfo);

#if BUILD_OBJECT_TRACKER
    layer_debug_messenger_actions(framework->report_data, framework->logging_messenger, pAllocator, "lunarg_object_tracker");
    object_tracker->report_data = framework->report_data;
#elif BUILD_THREAD_SAFETY
    layer_debug_messenger_actions(framework->report_data, framework->logging_messenger, pAllocator, "google_thread_checker");
    thread_checker->report_data = framework->report_data;
#elif BUILD_PARAMETER_VALIDATION
    layer_debug_messenger_actions(framework->report_data, framework->logging_messenger, pAllocator, "lunarg_parameter_validation");
    parameter_validation->report_data = framework->report_data;
#else
    layer_debug_messenger_actions(framework->report_data, framework->logging_messenger, pAllocator, "lunarg_unique_objects");
#endif

    for (auto intercept : framework->object_dispatch) {
        intercept->PostCallRecordCreateInstance(pCreateInfo, pAllocator, pInstance);
    }

    InstanceExtensionWhitelist(framework, pCreateInfo, *pInstance);

    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    dispatch_key key = get_dispatch_key(instance);
    auto layer_data = GetLayerDataPtr(key, layer_data_map);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallValidateDestroyInstance(instance, pAllocator);
        intercept->write_unlock();
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyInstance(instance, pAllocator);
        intercept->write_unlock();
    }

    layer_data->instance_dispatch_table.DestroyInstance(instance, pAllocator);

    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyInstance(instance, pAllocator);
        intercept->write_unlock();
    }
    // Clean up logging callback, if any
    while (layer_data->logging_messenger.size() > 0) {
        VkDebugUtilsMessengerEXT messenger = layer_data->logging_messenger.back();
        layer_destroy_messenger_callback(layer_data->report_data, messenger, pAllocator);
        layer_data->logging_messenger.pop_back();
    }
    while (layer_data->logging_callback.size() > 0) {
        VkDebugReportCallbackEXT callback = layer_data->logging_callback.back();
        layer_destroy_report_callback(layer_data->report_data, callback, pAllocator);
        layer_data->logging_callback.pop_back();
    }

    layer_debug_utils_destroy_instance(layer_data->report_data);

    for (auto item = layer_data->object_dispatch.begin(); item != layer_data->object_dispatch.end(); item++) {
        delete *item;
    }
    FreeLayerDataPtr(key, layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice gpu, const VkDeviceCreateInfo *pCreateInfo,
                                            const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    VkLayerDeviceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    auto instance_interceptor = GetLayerDataPtr(get_dispatch_key(gpu), layer_data_map);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(instance_interceptor->instance, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    // Get physical device limits for device
    VkPhysicalDeviceProperties device_properties = {};
    instance_interceptor->instance_dispatch_table.GetPhysicalDeviceProperties(gpu, &device_properties);

    // Setup the validation tables based on the application API version from the instance and the capabilities of the device driver
    uint32_t effective_api_version = std::min(device_properties.apiVersion, instance_interceptor->api_version);

    DeviceExtensions device_extensions = {};
    device_extensions.InitFromDeviceCreateInfo(&instance_interceptor->instance_extensions, effective_api_version, pCreateInfo);
    for (auto item : instance_interceptor->object_dispatch) {
        item->device_extensions = device_extensions;
    }

    bool skip = false;
    for (auto intercept : instance_interceptor->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : instance_interceptor->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
        intercept->write_unlock();
    }

    VkResult result = fpCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    auto device_interceptor = GetLayerDataPtr(get_dispatch_key(*pDevice), layer_data_map);

    // Save local info in device object
    device_interceptor->phys_dev_properties.properties = device_properties;
    device_interceptor->api_version = device_interceptor->device_extensions.InitFromDeviceCreateInfo(
        &instance_interceptor->instance_extensions, effective_api_version, pCreateInfo);
    device_interceptor->device_extensions = device_extensions;

    layer_init_device_dispatch_table(*pDevice, &device_interceptor->device_dispatch_table, fpGetDeviceProcAddr);

    device_interceptor->device = *pDevice;
    device_interceptor->physical_device = gpu;
    device_interceptor->instance = instance_interceptor->instance;
    device_interceptor->report_data = layer_debug_utils_create_device(instance_interceptor->report_data, *pDevice);

#if BUILD_OBJECT_TRACKER
    // Create child layer objects for this key and add to dispatch vector
    auto object_tracker = new ObjectLifetimes;
    // TODO:  Initialize child objects with parent info thru constuctor taking a parent object
    object_tracker->container_type = LayerObjectTypeObjectTracker;
    object_tracker->physical_device = gpu;
    object_tracker->instance = instance_interceptor->instance;
    object_tracker->report_data = device_interceptor->report_data;
    object_tracker->device_dispatch_table = device_interceptor->device_dispatch_table;
    object_tracker->api_version = device_interceptor->api_version;
    device_interceptor->object_dispatch.emplace_back(object_tracker);
#elif BUILD_THREAD_SAFETY
    auto thread_safety = new ThreadSafety;
    // TODO:  Initialize child objects with parent info thru constuctor taking a parent object
    thread_safety->container_type = LayerObjectTypeThreading;
    thread_safety->physical_device = gpu;
    thread_safety->instance = instance_interceptor->instance;
    thread_safety->report_data = device_interceptor->report_data;
    thread_safety->device_dispatch_table = device_interceptor->device_dispatch_table;
    thread_safety->api_version = device_interceptor->api_version;
    device_interceptor->object_dispatch.emplace_back(thread_safety);
#elif BUILD_PARAMETER_VALIDATION
    auto stateless_validation = new StatelessValidation;
    // TODO:  Initialize child objects with parent info thru constuctor taking a parent object
    stateless_validation->container_type = LayerObjectTypeParameterValidation;
    stateless_validation->physical_device = gpu;
    stateless_validation->instance = instance_interceptor->instance;
    stateless_validation->report_data = device_interceptor->report_data;
    stateless_validation->device_dispatch_table = device_interceptor->device_dispatch_table;
    stateless_validation->api_version = device_interceptor->api_version;
    device_interceptor->object_dispatch.emplace_back(stateless_validation);
#endif

    for (auto intercept : instance_interceptor->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
        intercept->write_unlock();
    }

    DeviceExtensionWhitelist(device_interceptor, pCreateInfo, *pDevice);

    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator) {
    dispatch_key key = get_dispatch_key(device);
    auto layer_data = GetLayerDataPtr(key, layer_data_map);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallValidateDestroyDevice(device, pAllocator);
        intercept->write_unlock();
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDevice(device, pAllocator);
        intercept->write_unlock();
    }
    layer_debug_utils_destroy_device(device);

    layer_data->device_dispatch_table.DestroyDevice(device, pAllocator);

    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDevice(device, pAllocator);
        intercept->write_unlock();
    }

    for (auto item = layer_data->object_dispatch.begin(); item != layer_data->object_dispatch.end(); item++) {
        delete *item;
    }
    FreeLayerDataPtr(key, layer_data_map);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugReportCallbackEXT(VkInstance instance,
                                                            const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                                                            const VkAllocationCallbacks *pAllocator,
                                                            VkDebugReportCallbackEXT *pCallback) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallValidateCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
        intercept->write_unlock();
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDebugReportCallbackEXT(layer_data, instance, pCreateInfo, pAllocator, pCallback);
    result = layer_create_report_callback(layer_data->report_data, false, pCreateInfo, pAllocator, pCallback);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
        intercept->write_unlock();
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                                         const VkAllocationCallbacks *pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallValidateDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
        intercept->write_unlock();
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDebugReportCallbackEXT(layer_data, instance, callback, pAllocator);
    layer_destroy_report_callback(layer_data->report_data, callback, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
        intercept->write_unlock();
    }
    VkResult result = DispatchEnumeratePhysicalDevices(layer_data, instance, pPhysicalDeviceCount, pPhysicalDevices);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFeatures(layer_data, physicalDevice, pFeatures);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFormatProperties(layer_data, physicalDevice, format, pFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceImageFormatProperties(layer_data, physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceProperties(physicalDevice, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceProperties(physicalDevice, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceProperties(layer_data, physicalDevice, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceProperties(physicalDevice, pProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceQueueFamilyProperties(layer_data, physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceMemoryProperties(layer_data, physicalDevice, pMemoryProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
        intercept->write_unlock();
    }
    DispatchGetDeviceQueue(layer_data, device, queueFamilyIndex, queueIndex, pQueue);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL QueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueSubmit(queue, submitCount, pSubmits, fence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueSubmit(queue, submitCount, pSubmits, fence);
        intercept->write_unlock();
    }
    VkResult result = DispatchQueueSubmit(layer_data, queue, submitCount, pSubmits, fence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordQueueSubmit(queue, submitCount, pSubmits, fence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL QueueWaitIdle(
    VkQueue                                     queue) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueWaitIdle(queue);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueWaitIdle(queue);
        intercept->write_unlock();
    }
    VkResult result = DispatchQueueWaitIdle(layer_data, queue);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordQueueWaitIdle(queue);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL DeviceWaitIdle(
    VkDevice                                    device) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDeviceWaitIdle(device);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDeviceWaitIdle(device);
        intercept->write_unlock();
    }
    VkResult result = DispatchDeviceWaitIdle(layer_data, device);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordDeviceWaitIdle(device);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
        intercept->write_unlock();
    }
    VkResult result = DispatchAllocateMemory(layer_data, device, pAllocateInfo, pAllocator, pMemory);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL FreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateFreeMemory(device, memory, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordFreeMemory(device, memory, pAllocator);
        intercept->write_unlock();
    }
    DispatchFreeMemory(layer_data, device, memory, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordFreeMemory(device, memory, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL MapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateMapMemory(device, memory, offset, size, flags, ppData);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordMapMemory(device, memory, offset, size, flags, ppData);
        intercept->write_unlock();
    }
    VkResult result = DispatchMapMemory(layer_data, device, memory, offset, size, flags, ppData);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordMapMemory(device, memory, offset, size, flags, ppData);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL UnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateUnmapMemory(device, memory);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordUnmapMemory(device, memory);
        intercept->write_unlock();
    }
    DispatchUnmapMemory(layer_data, device, memory);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordUnmapMemory(device, memory);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL FlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
        intercept->write_unlock();
    }
    VkResult result = DispatchFlushMappedMemoryRanges(layer_data, device, memoryRangeCount, pMemoryRanges);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL InvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
        intercept->write_unlock();
    }
    VkResult result = DispatchInvalidateMappedMemoryRanges(layer_data, device, memoryRangeCount, pMemoryRanges);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
        intercept->write_unlock();
    }
    DispatchGetDeviceMemoryCommitment(layer_data, device, memory, pCommittedMemoryInBytes);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindBufferMemory(device, buffer, memory, memoryOffset);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindBufferMemory(device, buffer, memory, memoryOffset);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindBufferMemory(layer_data, device, buffer, memory, memoryOffset);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindBufferMemory(device, buffer, memory, memoryOffset);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindImageMemory(device, image, memory, memoryOffset);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindImageMemory(device, image, memory, memoryOffset);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindImageMemory(layer_data, device, image, memory, memoryOffset);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindImageMemory(device, image, memory, memoryOffset);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetBufferMemoryRequirements(layer_data, device, buffer, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageMemoryRequirements(device, image, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageMemoryRequirements(device, image, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageMemoryRequirements(layer_data, device, image, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageMemoryRequirements(device, image, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageSparseMemoryRequirements(layer_data, device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceSparseImageFormatProperties(layer_data, physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL QueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
        intercept->write_unlock();
    }
    VkResult result = DispatchQueueBindSparse(layer_data, queue, bindInfoCount, pBindInfo, fence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordQueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateFence(device, pCreateInfo, pAllocator, pFence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateFence(device, pCreateInfo, pAllocator, pFence);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateFence(layer_data, device, pCreateInfo, pAllocator, pFence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateFence(device, pCreateInfo, pAllocator, pFence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyFence(device, fence, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyFence(device, fence, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyFence(layer_data, device, fence, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyFence(device, fence, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateResetFences(device, fenceCount, pFences);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordResetFences(device, fenceCount, pFences);
        intercept->write_unlock();
    }
    VkResult result = DispatchResetFences(layer_data, device, fenceCount, pFences);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordResetFences(device, fenceCount, pFences);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetFenceStatus(device, fence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetFenceStatus(device, fence);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetFenceStatus(layer_data, device, fence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetFenceStatus(device, fence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL WaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateWaitForFences(device, fenceCount, pFences, waitAll, timeout);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordWaitForFences(device, fenceCount, pFences, waitAll, timeout);
        intercept->write_unlock();
    }
    VkResult result = DispatchWaitForFences(layer_data, device, fenceCount, pFences, waitAll, timeout);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordWaitForFences(device, fenceCount, pFences, waitAll, timeout);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSemaphore(layer_data, device, pCreateInfo, pAllocator, pSemaphore);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySemaphore(device, semaphore, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySemaphore(device, semaphore, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySemaphore(layer_data, device, semaphore, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySemaphore(device, semaphore, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateEvent(device, pCreateInfo, pAllocator, pEvent);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateEvent(device, pCreateInfo, pAllocator, pEvent);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateEvent(layer_data, device, pCreateInfo, pAllocator, pEvent);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateEvent(device, pCreateInfo, pAllocator, pEvent);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyEvent(device, event, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyEvent(device, event, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyEvent(layer_data, device, event, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyEvent(device, event, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetEventStatus(device, event);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetEventStatus(device, event);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetEventStatus(layer_data, device, event);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetEventStatus(device, event);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateSetEvent(device, event);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordSetEvent(device, event);
        intercept->write_unlock();
    }
    VkResult result = DispatchSetEvent(layer_data, device, event);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordSetEvent(device, event);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ResetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateResetEvent(device, event);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordResetEvent(device, event);
        intercept->write_unlock();
    }
    VkResult result = DispatchResetEvent(layer_data, device, event);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordResetEvent(device, event);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateQueryPool(layer_data, device, pCreateInfo, pAllocator, pQueryPool);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyQueryPool(device, queryPool, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyQueryPool(device, queryPool, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyQueryPool(layer_data, device, queryPool, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyQueryPool(device, queryPool, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetQueryPoolResults(layer_data, device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateBuffer(layer_data, device, pCreateInfo, pAllocator, pBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyBuffer(device, buffer, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyBuffer(device, buffer, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyBuffer(layer_data, device, buffer, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyBuffer(device, buffer, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateBufferView(device, pCreateInfo, pAllocator, pView);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateBufferView(device, pCreateInfo, pAllocator, pView);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateBufferView(layer_data, device, pCreateInfo, pAllocator, pView);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateBufferView(device, pCreateInfo, pAllocator, pView);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyBufferView(device, bufferView, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyBufferView(device, bufferView, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyBufferView(layer_data, device, bufferView, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyBufferView(device, bufferView, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateImage(device, pCreateInfo, pAllocator, pImage);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateImage(device, pCreateInfo, pAllocator, pImage);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateImage(layer_data, device, pCreateInfo, pAllocator, pImage);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateImage(device, pCreateInfo, pAllocator, pImage);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyImage(device, image, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyImage(device, image, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyImage(layer_data, device, image, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyImage(device, image, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageSubresourceLayout(device, image, pSubresource, pLayout);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageSubresourceLayout(device, image, pSubresource, pLayout);
        intercept->write_unlock();
    }
    DispatchGetImageSubresourceLayout(layer_data, device, image, pSubresource, pLayout);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageSubresourceLayout(device, image, pSubresource, pLayout);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateImageView(device, pCreateInfo, pAllocator, pView);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateImageView(device, pCreateInfo, pAllocator, pView);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateImageView(layer_data, device, pCreateInfo, pAllocator, pView);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateImageView(device, pCreateInfo, pAllocator, pView);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyImageView(device, imageView, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyImageView(device, imageView, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyImageView(layer_data, device, imageView, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyImageView(device, imageView, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateShaderModule(layer_data, device, pCreateInfo, pAllocator, pShaderModule);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyShaderModule(device, shaderModule, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyShaderModule(device, shaderModule, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyShaderModule(layer_data, device, shaderModule, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyShaderModule(device, shaderModule, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreatePipelineCache(layer_data, device, pCreateInfo, pAllocator, pPipelineCache);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyPipelineCache(device, pipelineCache, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyPipelineCache(device, pipelineCache, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyPipelineCache(layer_data, device, pipelineCache, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyPipelineCache(device, pipelineCache, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPipelineCacheData(device, pipelineCache, pDataSize, pData);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPipelineCacheData(device, pipelineCache, pDataSize, pData);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPipelineCacheData(layer_data, device, pipelineCache, pDataSize, pData);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPipelineCacheData(device, pipelineCache, pDataSize, pData);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL MergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
        intercept->write_unlock();
    }
    VkResult result = DispatchMergePipelineCaches(layer_data, device, dstCache, srcCacheCount, pSrcCaches);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateGraphicsPipelines(layer_data, device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateComputePipelines(layer_data, device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyPipeline(device, pipeline, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyPipeline(device, pipeline, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyPipeline(layer_data, device, pipeline, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyPipeline(device, pipeline, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreatePipelineLayout(layer_data, device, pCreateInfo, pAllocator, pPipelineLayout);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyPipelineLayout(layer_data, device, pipelineLayout, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyPipelineLayout(device, pipelineLayout, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSampler(device, pCreateInfo, pAllocator, pSampler);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSampler(device, pCreateInfo, pAllocator, pSampler);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSampler(layer_data, device, pCreateInfo, pAllocator, pSampler);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSampler(device, pCreateInfo, pAllocator, pSampler);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySampler(device, sampler, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySampler(device, sampler, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySampler(layer_data, device, sampler, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySampler(device, sampler, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDescriptorSetLayout(layer_data, device, pCreateInfo, pAllocator, pSetLayout);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDescriptorSetLayout(layer_data, device, descriptorSetLayout, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDescriptorPool(layer_data, device, pCreateInfo, pAllocator, pDescriptorPool);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyDescriptorPool(device, descriptorPool, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDescriptorPool(device, descriptorPool, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDescriptorPool(layer_data, device, descriptorPool, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDescriptorPool(device, descriptorPool, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateResetDescriptorPool(device, descriptorPool, flags);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordResetDescriptorPool(device, descriptorPool, flags);
        intercept->write_unlock();
    }
    VkResult result = DispatchResetDescriptorPool(layer_data, device, descriptorPool, flags);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordResetDescriptorPool(device, descriptorPool, flags);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
        intercept->write_unlock();
    }
    VkResult result = DispatchAllocateDescriptorSets(layer_data, device, pAllocateInfo, pDescriptorSets);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL FreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
        intercept->write_unlock();
    }
    VkResult result = DispatchFreeDescriptorSets(layer_data, device, descriptorPool, descriptorSetCount, pDescriptorSets);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
        intercept->write_unlock();
    }
    DispatchUpdateDescriptorSets(layer_data, device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateFramebuffer(layer_data, device, pCreateInfo, pAllocator, pFramebuffer);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyFramebuffer(device, framebuffer, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyFramebuffer(device, framebuffer, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyFramebuffer(layer_data, device, framebuffer, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyFramebuffer(device, framebuffer, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateRenderPass(layer_data, device, pCreateInfo, pAllocator, pRenderPass);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyRenderPass(device, renderPass, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyRenderPass(device, renderPass, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyRenderPass(layer_data, device, renderPass, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyRenderPass(device, renderPass, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetRenderAreaGranularity(device, renderPass, pGranularity);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetRenderAreaGranularity(device, renderPass, pGranularity);
        intercept->write_unlock();
    }
    DispatchGetRenderAreaGranularity(layer_data, device, renderPass, pGranularity);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetRenderAreaGranularity(device, renderPass, pGranularity);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateCommandPool(layer_data, device, pCreateInfo, pAllocator, pCommandPool);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyCommandPool(device, commandPool, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyCommandPool(device, commandPool, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyCommandPool(layer_data, device, commandPool, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyCommandPool(device, commandPool, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolResetFlags                     flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateResetCommandPool(device, commandPool, flags);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordResetCommandPool(device, commandPool, flags);
        intercept->write_unlock();
    }
    VkResult result = DispatchResetCommandPool(layer_data, device, commandPool, flags);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordResetCommandPool(device, commandPool, flags);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
        intercept->write_unlock();
    }
    VkResult result = DispatchAllocateCommandBuffers(layer_data, device, pAllocateInfo, pCommandBuffers);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL FreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
    }
    DispatchFreeCommandBuffers(layer_data, device, commandPool, commandBufferCount, pCommandBuffers);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBeginCommandBuffer(commandBuffer, pBeginInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBeginCommandBuffer(commandBuffer, pBeginInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchBeginCommandBuffer(layer_data, commandBuffer, pBeginInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBeginCommandBuffer(commandBuffer, pBeginInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL EndCommandBuffer(
    VkCommandBuffer                             commandBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateEndCommandBuffer(commandBuffer);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordEndCommandBuffer(commandBuffer);
        intercept->write_unlock();
    }
    VkResult result = DispatchEndCommandBuffer(layer_data, commandBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordEndCommandBuffer(commandBuffer);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateResetCommandBuffer(commandBuffer, flags);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordResetCommandBuffer(commandBuffer, flags);
        intercept->write_unlock();
    }
    VkResult result = DispatchResetCommandBuffer(layer_data, commandBuffer, flags);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordResetCommandBuffer(commandBuffer, flags);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
        intercept->write_unlock();
    }
    DispatchCmdBindPipeline(layer_data, commandBuffer, pipelineBindPoint, pipeline);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
        intercept->write_unlock();
    }
    DispatchCmdSetViewport(layer_data, commandBuffer, firstViewport, viewportCount, pViewports);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
        intercept->write_unlock();
    }
    DispatchCmdSetScissor(layer_data, commandBuffer, firstScissor, scissorCount, pScissors);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetLineWidth(commandBuffer, lineWidth);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetLineWidth(commandBuffer, lineWidth);
        intercept->write_unlock();
    }
    DispatchCmdSetLineWidth(layer_data, commandBuffer, lineWidth);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetLineWidth(commandBuffer, lineWidth);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        intercept->write_unlock();
    }
    DispatchCmdSetDepthBias(layer_data, commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetBlendConstants(commandBuffer, blendConstants);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetBlendConstants(commandBuffer, blendConstants);
        intercept->write_unlock();
    }
    DispatchCmdSetBlendConstants(layer_data, commandBuffer, blendConstants);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetBlendConstants(commandBuffer, blendConstants);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
        intercept->write_unlock();
    }
    DispatchCmdSetDepthBounds(layer_data, commandBuffer, minDepthBounds, maxDepthBounds);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
        intercept->write_unlock();
    }
    DispatchCmdSetStencilCompareMask(layer_data, commandBuffer, faceMask, compareMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
        intercept->write_unlock();
    }
    DispatchCmdSetStencilWriteMask(layer_data, commandBuffer, faceMask, writeMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetStencilReference(commandBuffer, faceMask, reference);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetStencilReference(commandBuffer, faceMask, reference);
        intercept->write_unlock();
    }
    DispatchCmdSetStencilReference(layer_data, commandBuffer, faceMask, reference);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetStencilReference(commandBuffer, faceMask, reference);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
        intercept->write_unlock();
    }
    DispatchCmdBindDescriptorSets(layer_data, commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
        intercept->write_unlock();
    }
    DispatchCmdBindIndexBuffer(layer_data, commandBuffer, buffer, offset, indexType);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
        intercept->write_unlock();
    }
    DispatchCmdBindVertexBuffers(layer_data, commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
        intercept->write_unlock();
    }
    DispatchCmdDraw(layer_data, commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndexed(layer_data, commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndirect(layer_data, commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndexedIndirect(layer_data, commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
    DispatchCmdDispatch(layer_data, commandBuffer, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDispatchIndirect(commandBuffer, buffer, offset);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDispatchIndirect(commandBuffer, buffer, offset);
        intercept->write_unlock();
    }
    DispatchCmdDispatchIndirect(layer_data, commandBuffer, buffer, offset);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDispatchIndirect(commandBuffer, buffer, offset);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
    }
    DispatchCmdCopyBuffer(layer_data, commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
    DispatchCmdCopyImage(layer_data, commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
        intercept->write_unlock();
    }
    DispatchCmdBlitImage(layer_data, commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
    DispatchCmdCopyBufferToImage(layer_data, commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
    }
    DispatchCmdCopyImageToBuffer(layer_data, commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
        intercept->write_unlock();
    }
    DispatchCmdUpdateBuffer(layer_data, commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
        intercept->write_unlock();
    }
    DispatchCmdFillBuffer(layer_data, commandBuffer, dstBuffer, dstOffset, size, data);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
        intercept->write_unlock();
    }
    DispatchCmdClearColorImage(layer_data, commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
        intercept->write_unlock();
    }
    DispatchCmdClearDepthStencilImage(layer_data, commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
        intercept->write_unlock();
    }
    DispatchCmdClearAttachments(layer_data, commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
    DispatchCmdResolveImage(layer_data, commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
    }
    DispatchCmdSetEvent(layer_data, commandBuffer, event, stageMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdResetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdResetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
    }
    DispatchCmdResetEvent(layer_data, commandBuffer, event, stageMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdResetEvent(commandBuffer, event, stageMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
    }
    DispatchCmdWaitEvents(layer_data, commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
    }
    DispatchCmdPipelineBarrier(layer_data, commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginQuery(commandBuffer, queryPool, query, flags);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginQuery(commandBuffer, queryPool, query, flags);
        intercept->write_unlock();
    }
    DispatchCmdBeginQuery(layer_data, commandBuffer, queryPool, query, flags);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginQuery(commandBuffer, queryPool, query, flags);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndQuery(commandBuffer, queryPool, query);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndQuery(commandBuffer, queryPool, query);
        intercept->write_unlock();
    }
    DispatchCmdEndQuery(layer_data, commandBuffer, queryPool, query);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndQuery(commandBuffer, queryPool, query);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
        intercept->write_unlock();
    }
    DispatchCmdResetQueryPool(layer_data, commandBuffer, queryPool, firstQuery, queryCount);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
        intercept->write_unlock();
    }
    DispatchCmdWriteTimestamp(layer_data, commandBuffer, pipelineStage, queryPool, query);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
        intercept->write_unlock();
    }
    DispatchCmdCopyQueryPoolResults(layer_data, commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
        intercept->write_unlock();
    }
    DispatchCmdPushConstants(layer_data, commandBuffer, layout, stageFlags, offset, size, pValues);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
        intercept->write_unlock();
    }
    DispatchCmdBeginRenderPass(layer_data, commandBuffer, pRenderPassBegin, contents);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdNextSubpass(commandBuffer, contents);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdNextSubpass(commandBuffer, contents);
        intercept->write_unlock();
    }
    DispatchCmdNextSubpass(layer_data, commandBuffer, contents);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdNextSubpass(commandBuffer, contents);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass(
    VkCommandBuffer                             commandBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndRenderPass(commandBuffer);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndRenderPass(commandBuffer);
        intercept->write_unlock();
    }
    DispatchCmdEndRenderPass(layer_data, commandBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndRenderPass(commandBuffer);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
    }
    DispatchCmdExecuteCommands(layer_data, commandBuffer, commandBufferCount, pCommandBuffers);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindBufferMemory2(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindBufferMemory2(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindBufferMemory2(layer_data, device, bindInfoCount, pBindInfos);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindBufferMemory2(device, bindInfoCount, pBindInfos);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindImageMemory2(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindImageMemory2(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindImageMemory2(layer_data, device, bindInfoCount, pBindInfos);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindImageMemory2(device, bindInfoCount, pBindInfos);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
    }
    DispatchGetDeviceGroupPeerMemoryFeatures(layer_data, device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetDeviceMask(commandBuffer, deviceMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetDeviceMask(commandBuffer, deviceMask);
        intercept->write_unlock();
    }
    DispatchCmdSetDeviceMask(layer_data, commandBuffer, deviceMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetDeviceMask(commandBuffer, deviceMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
    DispatchCmdDispatchBase(layer_data, commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateEnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordEnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchEnumeratePhysicalDeviceGroups(layer_data, instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordEnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageMemoryRequirements2(layer_data, device, pInfo, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetBufferMemoryRequirements2(layer_data, device, pInfo, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageSparseMemoryRequirements2(layer_data, device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFeatures2(layer_data, physicalDevice, pFeatures);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceProperties2(physicalDevice, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceProperties2(physicalDevice, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceProperties2(layer_data, physicalDevice, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceProperties2(physicalDevice, pProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFormatProperties2(layer_data, physicalDevice, format, pFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceImageFormatProperties2(layer_data, physicalDevice, pImageFormatInfo, pImageFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceQueueFamilyProperties2(layer_data, physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceMemoryProperties2(layer_data, physicalDevice, pMemoryProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceSparseImageFormatProperties2(layer_data, physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL TrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateTrimCommandPool(device, commandPool, flags);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordTrimCommandPool(device, commandPool, flags);
        intercept->write_unlock();
    }
    DispatchTrimCommandPool(layer_data, device, commandPool, flags);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordTrimCommandPool(device, commandPool, flags);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceQueue2(device, pQueueInfo, pQueue);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceQueue2(device, pQueueInfo, pQueue);
        intercept->write_unlock();
    }
    DispatchGetDeviceQueue2(layer_data, device, pQueueInfo, pQueue);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDeviceQueue2(device, pQueueInfo, pQueue);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSamplerYcbcrConversion(layer_data, device, pCreateInfo, pAllocator, pYcbcrConversion);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySamplerYcbcrConversion(layer_data, device, ycbcrConversion, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDescriptorUpdateTemplate(layer_data, device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDescriptorUpdateTemplate(layer_data, device, descriptorUpdateTemplate, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateUpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordUpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
    }
    DispatchUpdateDescriptorSetWithTemplate(layer_data, device, descriptorSet, descriptorUpdateTemplate, pData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordUpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalBufferProperties(layer_data, physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalFenceProperties(layer_data, physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalSemaphoreProperties(layer_data, physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
        intercept->write_unlock();
    }
    DispatchGetDescriptorSetLayoutSupport(layer_data, device, pCreateInfo, pSupport);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySurfaceKHR(instance, surface, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySurfaceKHR(instance, surface, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySurfaceKHR(layer_data, instance, surface, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySurfaceKHR(instance, surface, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceSupportKHR(layer_data, physicalDevice, queueFamilyIndex, surface, pSupported);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceCapabilitiesKHR(layer_data, physicalDevice, surface, pSurfaceCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceFormatsKHR(layer_data, physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfacePresentModesKHR(layer_data, physicalDevice, surface, pPresentModeCount, pPresentModes);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSwapchainKHR(layer_data, device, pCreateInfo, pAllocator, pSwapchain);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySwapchainKHR(device, swapchain, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySwapchainKHR(device, swapchain, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySwapchainKHR(layer_data, device, swapchain, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySwapchainKHR(device, swapchain, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pSwapchainImageCount,
    VkImage*                                    pSwapchainImages) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetSwapchainImagesKHR(layer_data, device, swapchain, pSwapchainImageCount, pSwapchainImages);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
        intercept->write_unlock();
    }
    VkResult result = DispatchAcquireNextImageKHR(layer_data, device, swapchain, timeout, semaphore, fence, pImageIndex);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue                                     queue,
    const VkPresentInfoKHR*                     pPresentInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueuePresentKHR(queue, pPresentInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueuePresentKHR(queue, pPresentInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchQueuePresentKHR(layer_data, queue, pPresentInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordQueuePresentKHR(queue, pPresentInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDeviceGroupPresentCapabilitiesKHR(layer_data, device, pDeviceGroupPresentCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDeviceGroupSurfacePresentModesKHR(layer_data, device, surface, pModes);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDevicePresentRectanglesKHR(layer_data, physicalDevice, surface, pRectCount, pRects);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
        intercept->write_unlock();
    }
    VkResult result = DispatchAcquireNextImage2KHR(layer_data, device, pAcquireInfo, pImageIndex);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPropertiesKHR*                     pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceDisplayPropertiesKHR(layer_data, physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlanePropertiesKHR*                pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceDisplayPlanePropertiesKHR(layer_data, physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDisplayPlaneSupportedDisplaysKHR(layer_data, physicalDevice, planeIndex, pDisplayCount, pDisplays);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDisplayModePropertiesKHR(layer_data, physicalDevice, display, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDisplayModeKHR(layer_data, physicalDevice, display, pCreateInfo, pAllocator, pMode);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDisplayPlaneCapabilitiesKHR(layer_data, physicalDevice, mode, planeIndex, pCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDisplayPlaneSurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateSharedSwapchainsKHR(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainCreateInfoKHR*             pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchains) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSharedSwapchainsKHR(layer_data, device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
            intercept->write_unlock();
        }
    }
    return result;
}

#ifdef VK_USE_PLATFORM_XLIB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXlibSurfaceKHR(
    VkInstance                                  instance,
    const VkXlibSurfaceCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateXlibSurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    Display*                                    dpy,
    VisualID                                    visualID) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
        intercept->write_unlock();
        if (skip) return VK_FALSE;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
        intercept->write_unlock();
    }
    VkBool32 result = DispatchGetPhysicalDeviceXlibPresentationSupportKHR(layer_data, physicalDevice, queueFamilyIndex, dpy, visualID);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
        intercept->write_unlock();
    }
    return result;
}
#endif // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXcbSurfaceKHR(
    VkInstance                                  instance,
    const VkXcbSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateXcbSurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    xcb_connection_t*                           connection,
    xcb_visualid_t                              visual_id) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
        intercept->write_unlock();
        if (skip) return VK_FALSE;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
        intercept->write_unlock();
    }
    VkBool32 result = DispatchGetPhysicalDeviceXcbPresentationSupportKHR(layer_data, physicalDevice, queueFamilyIndex, connection, visual_id);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
        intercept->write_unlock();
    }
    return result;
}
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    const VkWaylandSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateWaylandSurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    struct wl_display*                          display) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
        intercept->write_unlock();
        if (skip) return VK_FALSE;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
        intercept->write_unlock();
    }
    VkBool32 result = DispatchGetPhysicalDeviceWaylandPresentationSupportKHR(layer_data, physicalDevice, queueFamilyIndex, display);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
        intercept->write_unlock();
    }
    return result;
}
#endif // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateAndroidSurfaceKHR(
    VkInstance                                  instance,
    const VkAndroidSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateAndroidSurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateAndroidSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_ANDROID_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateWin32SurfaceKHR(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
        intercept->write_unlock();
        if (skip) return VK_FALSE;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
        intercept->write_unlock();
    }
    VkBool32 result = DispatchGetPhysicalDeviceWin32PresentationSupportKHR(layer_data, physicalDevice, queueFamilyIndex);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
        intercept->write_unlock();
    }
    return result;
}
#endif // VK_USE_PLATFORM_WIN32_KHR




VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFeatures2KHR(layer_data, physicalDevice, pFeatures);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceProperties2KHR(layer_data, physicalDevice, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceFormatProperties2KHR(layer_data, physicalDevice, format, pFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceImageFormatProperties2KHR(layer_data, physicalDevice, pImageFormatInfo, pImageFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceQueueFamilyProperties2KHR(layer_data, physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceMemoryProperties2KHR(layer_data, physicalDevice, pMemoryProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceSparseImageFormatProperties2KHR(layer_data, physicalDevice, pFormatInfo, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
    }
    DispatchGetDeviceGroupPeerMemoryFeaturesKHR(layer_data, device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMaskKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetDeviceMaskKHR(commandBuffer, deviceMask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetDeviceMaskKHR(commandBuffer, deviceMask);
        intercept->write_unlock();
    }
    DispatchCmdSetDeviceMaskKHR(layer_data, commandBuffer, deviceMask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetDeviceMaskKHR(commandBuffer, deviceMask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDispatchBaseKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
    DispatchCmdDispatchBaseKHR(layer_data, commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
        intercept->write_unlock();
    }
}



VKAPI_ATTR void VKAPI_CALL TrimCommandPoolKHR(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateTrimCommandPoolKHR(device, commandPool, flags);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordTrimCommandPoolKHR(device, commandPool, flags);
        intercept->write_unlock();
    }
    DispatchTrimCommandPoolKHR(layer_data, device, commandPool, flags);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordTrimCommandPoolKHR(device, commandPool, flags);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroupsKHR(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateEnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordEnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchEnumeratePhysicalDeviceGroupsKHR(layer_data, instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordEnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalBufferPropertiesKHR(layer_data, physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
        intercept->write_unlock();
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryWin32HandleKHR(layer_data, device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryWin32HandlePropertiesKHR(layer_data, device, handleType, handle, pMemoryWin32HandleProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_WIN32_KHR


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryFdKHR(layer_data, device, pGetFdInfo, pFd);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryFdKHR(device, pGetFdInfo, pFd);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryFdPropertiesKHR(layer_data, device, handleType, fd, pMemoryFdProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalSemaphorePropertiesKHR(layer_data, physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
        intercept->write_unlock();
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchImportSemaphoreWin32HandleKHR(layer_data, device, pImportSemaphoreWin32HandleInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetSemaphoreWin32HandleKHR(layer_data, device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_WIN32_KHR


VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchImportSemaphoreFdKHR(layer_data, device, pImportSemaphoreFdInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetSemaphoreFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetSemaphoreFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetSemaphoreFdKHR(layer_data, device, pGetFdInfo, pFd);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetSemaphoreFdKHR(device, pGetFdInfo, pFd);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
        intercept->write_unlock();
    }
    DispatchCmdPushDescriptorSetKHR(layer_data, commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
        intercept->write_unlock();
    }
    DispatchCmdPushDescriptorSetWithTemplateKHR(layer_data, commandBuffer, descriptorUpdateTemplate, layout, set, pData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
        intercept->write_unlock();
    }
}





VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDescriptorUpdateTemplateKHR(layer_data, device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDescriptorUpdateTemplateKHR(layer_data, device, descriptorUpdateTemplate, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplateKHR(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateUpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordUpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
    }
    DispatchUpdateDescriptorSetWithTemplateKHR(layer_data, device, descriptorSet, descriptorUpdateTemplate, pData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordUpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass2KHR(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2KHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateRenderPass2KHR(layer_data, device, pCreateInfo, pAllocator, pRenderPass);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
        intercept->write_unlock();
    }
    DispatchCmdBeginRenderPass2KHR(layer_data, commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
        intercept->write_unlock();
    }
    DispatchCmdNextSubpass2KHR(layer_data, commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
        intercept->write_unlock();
    }
    DispatchCmdEndRenderPass2KHR(layer_data, commandBuffer, pSubpassEndInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetSwapchainStatusKHR(device, swapchain);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetSwapchainStatusKHR(device, swapchain);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetSwapchainStatusKHR(layer_data, device, swapchain);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetSwapchainStatusKHR(device, swapchain);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceExternalFencePropertiesKHR(layer_data, physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
        intercept->write_unlock();
    }
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchImportFenceWin32HandleKHR(layer_data, device, pImportFenceWin32HandleInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetFenceWin32HandleKHR(layer_data, device, pGetWin32HandleInfo, pHandle);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_WIN32_KHR


VKAPI_ATTR VkResult VKAPI_CALL ImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateImportFenceFdKHR(device, pImportFenceFdInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordImportFenceFdKHR(device, pImportFenceFdInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchImportFenceFdKHR(layer_data, device, pImportFenceFdInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordImportFenceFdKHR(device, pImportFenceFdInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetFenceFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetFenceFdKHR(device, pGetFdInfo, pFd);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetFenceFdKHR(layer_data, device, pGetFdInfo, pFd);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetFenceFdKHR(device, pGetFdInfo, pFd);
            intercept->write_unlock();
        }
    }
    return result;
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkSurfaceCapabilities2KHR*                  pSurfaceCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceCapabilities2KHR(layer_data, physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormat2KHR*                        pSurfaceFormats) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceFormats2KHR(layer_data, physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
            intercept->write_unlock();
        }
    }
    return result;
}



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayProperties2KHR*                    pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceDisplayProperties2KHR(layer_data, physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlaneProperties2KHR*               pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceDisplayPlaneProperties2KHR(layer_data, physicalDevice, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDisplayModeProperties2KHR(layer_data, physicalDevice, display, pPropertyCount, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkDisplayPlaneInfo2KHR*               pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR*             pCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetDisplayPlaneCapabilities2KHR(layer_data, physicalDevice, pDisplayPlaneInfo, pCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}





VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageMemoryRequirements2KHR(layer_data, device, pInfo, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetBufferMemoryRequirements2KHR(layer_data, device, pInfo, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetImageSparseMemoryRequirements2KHR(layer_data, device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
        intercept->write_unlock();
    }
}



VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversionKHR(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateSamplerYcbcrConversionKHR(layer_data, device, pCreateInfo, pAllocator, pYcbcrConversion);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversionKHR(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroySamplerYcbcrConversionKHR(layer_data, device, ycbcrConversion, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindBufferMemory2KHR(layer_data, device, bindInfoCount, pBindInfos);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindImageMemory2KHR(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindImageMemory2KHR(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindImageMemory2KHR(layer_data, device, bindInfoCount, pBindInfos);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindImageMemory2KHR(device, bindInfoCount, pBindInfos);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupportKHR(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
        intercept->write_unlock();
    }
    DispatchGetDescriptorSetLayoutSupportKHR(layer_data, device, pCreateInfo, pSupport);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndirectCountKHR(layer_data, commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndexedIndirectCountKHR(layer_data, commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
}









VKAPI_ATTR void VKAPI_CALL DebugReportMessageEXT(
    VkInstance                                  instance,
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        intercept->write_unlock();
    }
    DispatchDebugReportMessageEXT(layer_data, instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
        intercept->write_unlock();
    }
}








VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDebugMarkerSetObjectTagEXT(device, pTagInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDebugMarkerSetObjectTagEXT(device, pTagInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchDebugMarkerSetObjectTagEXT(layer_data, device, pTagInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordDebugMarkerSetObjectTagEXT(device, pTagInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDebugMarkerSetObjectNameEXT(device, pNameInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDebugMarkerSetObjectNameEXT(device, pNameInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchDebugMarkerSetObjectNameEXT(layer_data, device, pNameInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordDebugMarkerSetObjectNameEXT(device, pNameInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerBeginEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
    }
    DispatchCmdDebugMarkerBeginEXT(layer_data, commandBuffer, pMarkerInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerEndEXT(
    VkCommandBuffer                             commandBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDebugMarkerEndEXT(commandBuffer);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDebugMarkerEndEXT(commandBuffer);
        intercept->write_unlock();
    }
    DispatchCmdDebugMarkerEndEXT(layer_data, commandBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDebugMarkerEndEXT(commandBuffer);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerInsertEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
    }
    DispatchCmdDebugMarkerInsertEXT(layer_data, commandBuffer, pMarkerInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
        intercept->write_unlock();
    }
}




VKAPI_ATTR void VKAPI_CALL CmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
        intercept->write_unlock();
    }
    DispatchCmdBindTransformFeedbackBuffersEXT(layer_data, commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
    }
    DispatchCmdBeginTransformFeedbackEXT(layer_data, commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
    }
    DispatchCmdEndTransformFeedbackEXT(layer_data, commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags,
    uint32_t                                    index) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
        intercept->write_unlock();
    }
    DispatchCmdBeginQueryIndexedEXT(layer_data, commandBuffer, queryPool, query, flags, index);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    uint32_t                                    index) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
        intercept->write_unlock();
    }
    DispatchCmdEndQueryIndexedEXT(layer_data, commandBuffer, queryPool, query, index);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectByteCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkBuffer                                    counterBuffer,
    VkDeviceSize                                counterBufferOffset,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndirectByteCountEXT(layer_data, commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndirectCountAMD(layer_data, commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawIndexedIndirectCountAMD(layer_data, commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
}






VKAPI_ATTR VkResult VKAPI_CALL GetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetShaderInfoAMD(layer_data, device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
            intercept->write_unlock();
        }
    }
    return result;
}





VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkExternalMemoryHandleTypeFlagsNV           externalHandleType,
    VkExternalImageFormatPropertiesNV*          pExternalImageFormatProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceExternalImageFormatPropertiesNV(layer_data, physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
            intercept->write_unlock();
        }
    }
    return result;
}


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryWin32HandleNV(device, memory, handleType, pHandle);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryWin32HandleNV(device, memory, handleType, pHandle);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryWin32HandleNV(layer_data, device, memory, handleType, pHandle);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryWin32HandleNV(device, memory, handleType, pHandle);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR


#ifdef VK_USE_PLATFORM_VI_NN

VKAPI_ATTR VkResult VKAPI_CALL CreateViSurfaceNN(
    VkInstance                                  instance,
    const VkViSurfaceCreateInfoNN*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateViSurfaceNN(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_VI_NN





VKAPI_ATTR void VKAPI_CALL CmdBeginConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfoEXT*   pConditionalRenderingBegin) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
        intercept->write_unlock();
    }
    DispatchCmdBeginConditionalRenderingEXT(layer_data, commandBuffer, pConditionalRenderingBegin);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndConditionalRenderingEXT(commandBuffer);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndConditionalRenderingEXT(commandBuffer);
        intercept->write_unlock();
    }
    DispatchCmdEndConditionalRenderingEXT(layer_data, commandBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndConditionalRenderingEXT(commandBuffer);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL CmdProcessCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdProcessCommandsInfoNVX*          pProcessCommandsInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdProcessCommandsNVX(commandBuffer, pProcessCommandsInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdProcessCommandsNVX(commandBuffer, pProcessCommandsInfo);
        intercept->write_unlock();
    }
    DispatchCmdProcessCommandsNVX(layer_data, commandBuffer, pProcessCommandsInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdProcessCommandsNVX(commandBuffer, pProcessCommandsInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdReserveSpaceForCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdReserveSpaceForCommandsInfoNVX*  pReserveSpaceInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdReserveSpaceForCommandsNVX(commandBuffer, pReserveSpaceInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdReserveSpaceForCommandsNVX(commandBuffer, pReserveSpaceInfo);
        intercept->write_unlock();
    }
    DispatchCmdReserveSpaceForCommandsNVX(layer_data, commandBuffer, pReserveSpaceInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdReserveSpaceForCommandsNVX(commandBuffer, pReserveSpaceInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNVX*                pIndirectCommandsLayout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateIndirectCommandsLayoutNVX(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateIndirectCommandsLayoutNVX(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateIndirectCommandsLayoutNVX(layer_data, device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateIndirectCommandsLayoutNVX(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNVX                 indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyIndirectCommandsLayoutNVX(device, indirectCommandsLayout, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyIndirectCommandsLayoutNVX(device, indirectCommandsLayout, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyIndirectCommandsLayoutNVX(layer_data, device, indirectCommandsLayout, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyIndirectCommandsLayoutNVX(device, indirectCommandsLayout, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateObjectTableNVX(
    VkDevice                                    device,
    const VkObjectTableCreateInfoNVX*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkObjectTableNVX*                           pObjectTable) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateObjectTableNVX(device, pCreateInfo, pAllocator, pObjectTable);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateObjectTableNVX(device, pCreateInfo, pAllocator, pObjectTable);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateObjectTableNVX(layer_data, device, pCreateInfo, pAllocator, pObjectTable);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateObjectTableNVX(device, pCreateInfo, pAllocator, pObjectTable);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyObjectTableNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyObjectTableNVX(device, objectTable, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyObjectTableNVX(device, objectTable, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyObjectTableNVX(layer_data, device, objectTable, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyObjectTableNVX(device, objectTable, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectTableEntryNVX* const*         ppObjectTableEntries,
    const uint32_t*                             pObjectIndices) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateRegisterObjectsNVX(device, objectTable, objectCount, ppObjectTableEntries, pObjectIndices);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordRegisterObjectsNVX(device, objectTable, objectCount, ppObjectTableEntries, pObjectIndices);
        intercept->write_unlock();
    }
    VkResult result = DispatchRegisterObjectsNVX(layer_data, device, objectTable, objectCount, ppObjectTableEntries, pObjectIndices);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordRegisterObjectsNVX(device, objectTable, objectCount, ppObjectTableEntries, pObjectIndices);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL UnregisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectEntryTypeNVX*                 pObjectEntryTypes,
    const uint32_t*                             pObjectIndices) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateUnregisterObjectsNVX(device, objectTable, objectCount, pObjectEntryTypes, pObjectIndices);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordUnregisterObjectsNVX(device, objectTable, objectCount, pObjectEntryTypes, pObjectIndices);
        intercept->write_unlock();
    }
    VkResult result = DispatchUnregisterObjectsNVX(layer_data, device, objectTable, objectCount, pObjectEntryTypes, pObjectIndices);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordUnregisterObjectsNVX(device, objectTable, objectCount, pObjectEntryTypes, pObjectIndices);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceGeneratedCommandsPropertiesNVX(
    VkPhysicalDevice                            physicalDevice,
    VkDeviceGeneratedCommandsFeaturesNVX*       pFeatures,
    VkDeviceGeneratedCommandsLimitsNVX*         pLimits) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceGeneratedCommandsPropertiesNVX(physicalDevice, pFeatures, pLimits);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceGeneratedCommandsPropertiesNVX(physicalDevice, pFeatures, pLimits);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceGeneratedCommandsPropertiesNVX(layer_data, physicalDevice, pFeatures, pLimits);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceGeneratedCommandsPropertiesNVX(physicalDevice, pFeatures, pLimits);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL CmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
        intercept->write_unlock();
    }
    DispatchCmdSetViewportWScalingNV(layer_data, commandBuffer, firstViewport, viewportCount, pViewportWScalings);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL ReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateReleaseDisplayEXT(physicalDevice, display);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordReleaseDisplayEXT(physicalDevice, display);
        intercept->write_unlock();
    }
    VkResult result = DispatchReleaseDisplayEXT(layer_data, physicalDevice, display);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordReleaseDisplayEXT(physicalDevice, display);
            intercept->write_unlock();
        }
    }
    return result;
}

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT

VKAPI_ATTR VkResult VKAPI_CALL AcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateAcquireXlibDisplayEXT(physicalDevice, dpy, display);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordAcquireXlibDisplayEXT(physicalDevice, dpy, display);
        intercept->write_unlock();
    }
    VkResult result = DispatchAcquireXlibDisplayEXT(layer_data, physicalDevice, dpy, display);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordAcquireXlibDisplayEXT(physicalDevice, dpy, display);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRandROutputDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    RROutput                                    rrOutput,
    VkDisplayKHR*                               pDisplay) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetRandROutputDisplayEXT(layer_data, physicalDevice, dpy, rrOutput, pDisplay);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceSurfaceCapabilities2EXT(layer_data, physicalDevice, surface, pSurfaceCapabilities);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL DisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDisplayPowerControlEXT(device, display, pDisplayPowerInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDisplayPowerControlEXT(device, display, pDisplayPowerInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchDisplayPowerControlEXT(layer_data, device, display, pDisplayPowerInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordDisplayPowerControlEXT(device, display, pDisplayPowerInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateRegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordRegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
        intercept->write_unlock();
    }
    VkResult result = DispatchRegisterDeviceEventEXT(layer_data, device, pDeviceEventInfo, pAllocator, pFence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordRegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateRegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordRegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
        intercept->write_unlock();
    }
    VkResult result = DispatchRegisterDisplayEventEXT(layer_data, device, display, pDisplayEventInfo, pAllocator, pFence);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordRegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetSwapchainCounterEXT(layer_data, device, swapchain, counter, pCounterValue);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL GetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetRefreshCycleDurationGOOGLE(layer_data, device, swapchain, pDisplayTimingProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPastPresentationTimingGOOGLE(layer_data, device, swapchain, pPresentationTimingCount, pPresentationTimings);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
            intercept->write_unlock();
        }
    }
    return result;
}







VKAPI_ATTR void VKAPI_CALL CmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
        intercept->write_unlock();
    }
    DispatchCmdSetDiscardRectangleEXT(layer_data, commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
        intercept->write_unlock();
    }
}




VKAPI_ATTR void VKAPI_CALL SetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateSetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordSetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
        intercept->write_unlock();
    }
    DispatchSetHdrMetadataEXT(layer_data, device, swapchainCount, pSwapchains, pMetadata);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordSetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
        intercept->write_unlock();
    }
}

#ifdef VK_USE_PLATFORM_IOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateIOSSurfaceMVK(
    VkInstance                                  instance,
    const VkIOSSurfaceCreateInfoMVK*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateIOSSurfaceMVK(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_MACOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateMacOSSurfaceMVK(
    VkInstance                                  instance,
    const VkMacOSSurfaceCreateInfoMVK*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateMacOSSurfaceMVK(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_MACOS_MVK




VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateSetDebugUtilsObjectNameEXT(device, pNameInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordSetDebugUtilsObjectNameEXT(device, pNameInfo);
        intercept->write_unlock();
    }
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        layer_data->report_data->debugUtilsObjectNameMap->insert(std::make_pair<uint64_t, std::string>((uint64_t &&) pNameInfo->objectHandle, pNameInfo->pObjectName));
    }
    VkResult result = DispatchSetDebugUtilsObjectNameEXT(layer_data, device, pNameInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordSetDebugUtilsObjectNameEXT(device, pNameInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateSetDebugUtilsObjectTagEXT(device, pTagInfo);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordSetDebugUtilsObjectTagEXT(device, pTagInfo);
        intercept->write_unlock();
    }
    VkResult result = DispatchSetDebugUtilsObjectTagEXT(layer_data, device, pTagInfo);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordSetDebugUtilsObjectTagEXT(device, pTagInfo);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
    }
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        BeginQueueDebugUtilsLabel(layer_data->report_data, queue, pLabelInfo);
    }
    DispatchQueueBeginDebugUtilsLabelEXT(layer_data, queue, pLabelInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueEndDebugUtilsLabelEXT(queue);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueEndDebugUtilsLabelEXT(queue);
        intercept->write_unlock();
    }
    DispatchQueueEndDebugUtilsLabelEXT(layer_data, queue);
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        EndQueueDebugUtilsLabel(layer_data->report_data, queue);
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordQueueEndDebugUtilsLabelEXT(queue);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
    }
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        InsertQueueDebugUtilsLabel(layer_data->report_data, queue, pLabelInfo);
    }
    DispatchQueueInsertDebugUtilsLabelEXT(layer_data, queue, pLabelInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
    }
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        BeginCmdDebugUtilsLabel(layer_data->report_data, commandBuffer, pLabelInfo);
    }
    DispatchCmdBeginDebugUtilsLabelEXT(layer_data, commandBuffer, pLabelInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdEndDebugUtilsLabelEXT(commandBuffer);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdEndDebugUtilsLabelEXT(commandBuffer);
        intercept->write_unlock();
    }
    DispatchCmdEndDebugUtilsLabelEXT(layer_data, commandBuffer);
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        EndCmdDebugUtilsLabel(layer_data->report_data, commandBuffer);
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdEndDebugUtilsLabelEXT(commandBuffer);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
    }
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        InsertCmdDebugUtilsLabel(layer_data->report_data, commandBuffer, pLabelInfo);
    }
    DispatchCmdInsertDebugUtilsLabelEXT(layer_data, commandBuffer, pLabelInfo);
    {
        std::lock_guard<std::mutex> lock(layer_data->validation_object_mutex);
        InsertCmdDebugUtilsLabel(layer_data->report_data, commandBuffer, pLabelInfo);
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateDebugUtilsMessengerEXT(layer_data, instance, pCreateInfo, pAllocator, pMessenger);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyDebugUtilsMessengerEXT(layer_data, instance, messenger, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL SubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
        intercept->write_unlock();
    }
    DispatchSubmitDebugUtilsMessageEXT(layer_data, instance, messageSeverity, messageTypes, pCallbackData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
        intercept->write_unlock();
    }
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetAndroidHardwareBufferPropertiesANDROID(
    VkDevice                                    device,
    const struct AHardwareBuffer*               buffer,
    VkAndroidHardwareBufferPropertiesANDROID*   pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetAndroidHardwareBufferPropertiesANDROID(layer_data, device, buffer, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetAndroidHardwareBufferPropertiesANDROID(device, buffer, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryAndroidHardwareBufferANDROID(
    VkDevice                                    device,
    const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo,
    struct AHardwareBuffer**                    pBuffer) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryAndroidHardwareBufferANDROID(layer_data, device, pInfo, pBuffer);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryAndroidHardwareBufferANDROID(device, pInfo, pBuffer);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_ANDROID_KHR








VKAPI_ATTR void VKAPI_CALL CmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
        intercept->write_unlock();
    }
    DispatchCmdSetSampleLocationsEXT(layer_data, commandBuffer, pSampleLocationsInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
        intercept->write_unlock();
    }
    DispatchGetPhysicalDeviceMultisamplePropertiesEXT(layer_data, physicalDevice, samples, pMultisampleProperties);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
        intercept->write_unlock();
    }
}







VKAPI_ATTR VkResult VKAPI_CALL GetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetImageDrmFormatModifierPropertiesEXT(layer_data, device, image, pProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR VkResult VKAPI_CALL CreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateValidationCacheEXT(layer_data, device, pCreateInfo, pAllocator, pValidationCache);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyValidationCacheEXT(device, validationCache, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyValidationCacheEXT(device, validationCache, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyValidationCacheEXT(layer_data, device, validationCache, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyValidationCacheEXT(device, validationCache, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL MergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateMergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordMergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
        intercept->write_unlock();
    }
    VkResult result = DispatchMergeValidationCachesEXT(layer_data, device, dstCache, srcCacheCount, pSrcCaches);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordMergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetValidationCacheDataEXT(layer_data, device, validationCache, pDataSize, pData);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
            intercept->write_unlock();
        }
    }
    return result;
}




VKAPI_ATTR void VKAPI_CALL CmdBindShadingRateImageNV(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
        intercept->write_unlock();
    }
    DispatchCmdBindShadingRateImageNV(layer_data, commandBuffer, imageView, imageLayout);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkShadingRatePaletteNV*               pShadingRatePalettes) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
        intercept->write_unlock();
    }
    DispatchCmdSetViewportShadingRatePaletteNV(layer_data, commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdSetCoarseSampleOrderNV(
    VkCommandBuffer                             commandBuffer,
    VkCoarseSampleOrderTypeNV                   sampleOrderType,
    uint32_t                                    customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV*          pCustomSampleOrders) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
        intercept->write_unlock();
    }
    DispatchCmdSetCoarseSampleOrderNV(layer_data, commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL CreateAccelerationStructureNV(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureNV*                  pAccelerationStructure) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateAccelerationStructureNV(layer_data, device, pCreateInfo, pAllocator, pAccelerationStructure);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL DestroyAccelerationStructureNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
        intercept->write_unlock();
    }
    DispatchDestroyAccelerationStructureNV(layer_data, device, accelerationStructure, pAllocator);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetAccelerationStructureMemoryRequirementsNV(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR*                   pMemoryRequirements) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
    DispatchGetAccelerationStructureMemoryRequirementsNV(layer_data, device, pInfo, pMemoryRequirements);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL BindAccelerationStructureMemoryNV(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
        intercept->write_unlock();
    }
    VkResult result = DispatchBindAccelerationStructureMemoryNV(layer_data, device, bindInfoCount, pBindInfos);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureInfoNV*        pInfo,
    VkBuffer                                    instanceData,
    VkDeviceSize                                instanceOffset,
    VkBool32                                    update,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkBuffer                                    scratch,
    VkDeviceSize                                scratchOffset) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
        intercept->write_unlock();
    }
    DispatchCmdBuildAccelerationStructureNV(layer_data, commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkCopyAccelerationStructureModeNV           mode) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
        intercept->write_unlock();
    }
    DispatchCmdCopyAccelerationStructureNV(layer_data, commandBuffer, dst, src, mode);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    raygenShaderBindingTableBuffer,
    VkDeviceSize                                raygenShaderBindingOffset,
    VkBuffer                                    missShaderBindingTableBuffer,
    VkDeviceSize                                missShaderBindingOffset,
    VkDeviceSize                                missShaderBindingStride,
    VkBuffer                                    hitShaderBindingTableBuffer,
    VkDeviceSize                                hitShaderBindingOffset,
    VkDeviceSize                                hitShaderBindingStride,
    VkBuffer                                    callableShaderBindingTableBuffer,
    VkDeviceSize                                callableShaderBindingOffset,
    VkDeviceSize                                callableShaderBindingStride,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
        intercept->write_unlock();
    }
    DispatchCmdTraceRaysNV(layer_data, commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRayTracingPipelinesNV(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateRayTracingPipelinesNV(layer_data, device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingShaderGroupHandlesNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetRayTracingShaderGroupHandlesNV(layer_data, device, pipeline, firstGroup, groupCount, dataSize, pData);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetAccelerationStructureHandleNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    size_t                                      dataSize,
    void*                                       pData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetAccelerationStructureHandleNV(layer_data, device, accelerationStructure, dataSize, pData);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_CALL CmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureNV*            pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
        intercept->write_unlock();
    }
    DispatchCmdWriteAccelerationStructuresPropertiesNV(layer_data, commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
        intercept->write_unlock();
    }
}

VKAPI_ATTR VkResult VKAPI_CALL CompileDeferredNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    shader) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCompileDeferredNV(device, pipeline, shader);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCompileDeferredNV(device, pipeline, shader);
        intercept->write_unlock();
    }
    VkResult result = DispatchCompileDeferredNV(layer_data, device, pipeline, shader);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCompileDeferredNV(device, pipeline, shader);
            intercept->write_unlock();
        }
    }
    return result;
}




VKAPI_ATTR VkResult VKAPI_CALL GetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetMemoryHostPointerPropertiesEXT(layer_data, device, handleType, pHostPointer, pMemoryHostPointerProperties);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
            intercept->write_unlock();
        }
    }
    return result;
}


VKAPI_ATTR void VKAPI_CALL CmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
        intercept->write_unlock();
    }
    DispatchCmdWriteBufferMarkerAMD(layer_data, commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
        intercept->write_unlock();
    }
}


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pTimeDomainCount,
    VkTimeDomainEXT*                            pTimeDomains) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(physicalDevice), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetPhysicalDeviceCalibrateableTimeDomainsEXT(layer_data, physicalDevice, pTimeDomainCount, pTimeDomains);
    for (auto intercept : layer_data->object_dispatch) {
        if (((VK_SUCCESS == result) || (VK_INCOMPLETE == result)) || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
            intercept->write_unlock();
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
        intercept->write_unlock();
    }
    VkResult result = DispatchGetCalibratedTimestampsEXT(layer_data, device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
            intercept->write_unlock();
        }
    }
    return result;
}







VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    taskCount,
    uint32_t                                    firstTask) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
        intercept->write_unlock();
    }
    DispatchCmdDrawMeshTasksNV(layer_data, commandBuffer, taskCount, firstTask);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawMeshTasksIndirectNV(layer_data, commandBuffer, buffer, offset, drawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
    DispatchCmdDrawMeshTasksIndirectCountNV(layer_data, commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
        intercept->write_unlock();
    }
}




VKAPI_ATTR void VKAPI_CALL CmdSetExclusiveScissorNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstExclusiveScissor,
    uint32_t                                    exclusiveScissorCount,
    const VkRect2D*                             pExclusiveScissors) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
        intercept->write_unlock();
    }
    DispatchCmdSetExclusiveScissorNV(layer_data, commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
        intercept->write_unlock();
    }
}


VKAPI_ATTR void VKAPI_CALL CmdSetCheckpointNV(
    VkCommandBuffer                             commandBuffer,
    const void*                                 pCheckpointMarker) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(commandBuffer), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
        intercept->write_unlock();
    }
    DispatchCmdSetCheckpointNV(layer_data, commandBuffer, pCheckpointMarker);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
        intercept->write_unlock();
    }
}

VKAPI_ATTR void VKAPI_CALL GetQueueCheckpointDataNV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointDataNV*                         pCheckpointData) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(queue), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
        intercept->write_unlock();
        if (skip) return;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
        intercept->write_unlock();
    }
    DispatchGetQueueCheckpointDataNV(layer_data, queue, pCheckpointDataCount, pCheckpointData);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
        intercept->write_unlock();
    }
}


#ifdef VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL CreateImagePipeSurfaceFUCHSIA(
    VkInstance                                  instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(instance), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateCreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
        if (skip) return VK_ERROR_VALIDATION_FAILED_EXT;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordCreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
        intercept->write_unlock();
    }
    VkResult result = DispatchCreateImagePipeSurfaceFUCHSIA(layer_data, instance, pCreateInfo, pAllocator, pSurface);
    for (auto intercept : layer_data->object_dispatch) {
        if ((VK_SUCCESS == result)  || (intercept->container_type == LayerObjectTypeThreading)) {
            intercept->write_lock();
            intercept->PostCallRecordCreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
            intercept->write_unlock();
        }
    }
    return result;
}
#endif // VK_USE_PLATFORM_FUCHSIA








VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetBufferDeviceAddressEXT(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfoEXT*         pInfo) {
    auto layer_data = GetLayerDataPtr(get_dispatch_key(device), layer_data_map);
    bool skip = false;
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        skip |= intercept->PreCallValidateGetBufferDeviceAddressEXT(device, pInfo);
        intercept->write_unlock();
        if (skip) return 0;
    }
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PreCallRecordGetBufferDeviceAddressEXT(device, pInfo);
        intercept->write_unlock();
    }
    VkDeviceAddress result = DispatchGetBufferDeviceAddressEXT(layer_data, device, pInfo);
    for (auto intercept : layer_data->object_dispatch) {
        intercept->write_lock();
        intercept->PostCallRecordGetBufferDeviceAddressEXT(device, pInfo);
        intercept->write_unlock();
    }
    return result;
}



// Map of all APIs to be intercepted by this layer
const std::unordered_map<std::string, void*> name_to_funcptr_map = {
    {"vkCreateInstance", (void*)CreateInstance},
    {"vkDestroyInstance", (void*)DestroyInstance},
    {"vkEnumeratePhysicalDevices", (void*)EnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceFeatures", (void*)GetPhysicalDeviceFeatures},
    {"vkGetPhysicalDeviceFormatProperties", (void*)GetPhysicalDeviceFormatProperties},
    {"vkGetPhysicalDeviceImageFormatProperties", (void*)GetPhysicalDeviceImageFormatProperties},
    {"vkGetPhysicalDeviceProperties", (void*)GetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceQueueFamilyProperties", (void*)GetPhysicalDeviceQueueFamilyProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (void*)GetPhysicalDeviceMemoryProperties},
    {"vkGetInstanceProcAddr", (void*)GetInstanceProcAddr},
    {"vkGetDeviceProcAddr", (void*)GetDeviceProcAddr},
    {"vkCreateDevice", (void*)CreateDevice},
    {"vkDestroyDevice", (void*)DestroyDevice},
    {"vkEnumerateInstanceExtensionProperties", (void*)EnumerateInstanceExtensionProperties},
    {"vkEnumerateDeviceExtensionProperties", (void*)EnumerateDeviceExtensionProperties},
    {"vkEnumerateInstanceLayerProperties", (void*)EnumerateInstanceLayerProperties},
    {"vkEnumerateDeviceLayerProperties", (void*)EnumerateDeviceLayerProperties},
    {"vkGetDeviceQueue", (void*)GetDeviceQueue},
    {"vkQueueSubmit", (void*)QueueSubmit},
    {"vkQueueWaitIdle", (void*)QueueWaitIdle},
    {"vkDeviceWaitIdle", (void*)DeviceWaitIdle},
    {"vkAllocateMemory", (void*)AllocateMemory},
    {"vkFreeMemory", (void*)FreeMemory},
    {"vkMapMemory", (void*)MapMemory},
    {"vkUnmapMemory", (void*)UnmapMemory},
    {"vkFlushMappedMemoryRanges", (void*)FlushMappedMemoryRanges},
    {"vkInvalidateMappedMemoryRanges", (void*)InvalidateMappedMemoryRanges},
    {"vkGetDeviceMemoryCommitment", (void*)GetDeviceMemoryCommitment},
    {"vkBindBufferMemory", (void*)BindBufferMemory},
    {"vkBindImageMemory", (void*)BindImageMemory},
    {"vkGetBufferMemoryRequirements", (void*)GetBufferMemoryRequirements},
    {"vkGetImageMemoryRequirements", (void*)GetImageMemoryRequirements},
    {"vkGetImageSparseMemoryRequirements", (void*)GetImageSparseMemoryRequirements},
    {"vkGetPhysicalDeviceSparseImageFormatProperties", (void*)GetPhysicalDeviceSparseImageFormatProperties},
    {"vkQueueBindSparse", (void*)QueueBindSparse},
    {"vkCreateFence", (void*)CreateFence},
    {"vkDestroyFence", (void*)DestroyFence},
    {"vkResetFences", (void*)ResetFences},
    {"vkGetFenceStatus", (void*)GetFenceStatus},
    {"vkWaitForFences", (void*)WaitForFences},
    {"vkCreateSemaphore", (void*)CreateSemaphore},
    {"vkDestroySemaphore", (void*)DestroySemaphore},
    {"vkCreateEvent", (void*)CreateEvent},
    {"vkDestroyEvent", (void*)DestroyEvent},
    {"vkGetEventStatus", (void*)GetEventStatus},
    {"vkSetEvent", (void*)SetEvent},
    {"vkResetEvent", (void*)ResetEvent},
    {"vkCreateQueryPool", (void*)CreateQueryPool},
    {"vkDestroyQueryPool", (void*)DestroyQueryPool},
    {"vkGetQueryPoolResults", (void*)GetQueryPoolResults},
    {"vkCreateBuffer", (void*)CreateBuffer},
    {"vkDestroyBuffer", (void*)DestroyBuffer},
    {"vkCreateBufferView", (void*)CreateBufferView},
    {"vkDestroyBufferView", (void*)DestroyBufferView},
    {"vkCreateImage", (void*)CreateImage},
    {"vkDestroyImage", (void*)DestroyImage},
    {"vkGetImageSubresourceLayout", (void*)GetImageSubresourceLayout},
    {"vkCreateImageView", (void*)CreateImageView},
    {"vkDestroyImageView", (void*)DestroyImageView},
    {"vkCreateShaderModule", (void*)CreateShaderModule},
    {"vkDestroyShaderModule", (void*)DestroyShaderModule},
    {"vkCreatePipelineCache", (void*)CreatePipelineCache},
    {"vkDestroyPipelineCache", (void*)DestroyPipelineCache},
    {"vkGetPipelineCacheData", (void*)GetPipelineCacheData},
    {"vkMergePipelineCaches", (void*)MergePipelineCaches},
    {"vkCreateGraphicsPipelines", (void*)CreateGraphicsPipelines},
    {"vkCreateComputePipelines", (void*)CreateComputePipelines},
    {"vkDestroyPipeline", (void*)DestroyPipeline},
    {"vkCreatePipelineLayout", (void*)CreatePipelineLayout},
    {"vkDestroyPipelineLayout", (void*)DestroyPipelineLayout},
    {"vkCreateSampler", (void*)CreateSampler},
    {"vkDestroySampler", (void*)DestroySampler},
    {"vkCreateDescriptorSetLayout", (void*)CreateDescriptorSetLayout},
    {"vkDestroyDescriptorSetLayout", (void*)DestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (void*)CreateDescriptorPool},
    {"vkDestroyDescriptorPool", (void*)DestroyDescriptorPool},
    {"vkResetDescriptorPool", (void*)ResetDescriptorPool},
    {"vkAllocateDescriptorSets", (void*)AllocateDescriptorSets},
    {"vkFreeDescriptorSets", (void*)FreeDescriptorSets},
    {"vkUpdateDescriptorSets", (void*)UpdateDescriptorSets},
    {"vkCreateFramebuffer", (void*)CreateFramebuffer},
    {"vkDestroyFramebuffer", (void*)DestroyFramebuffer},
    {"vkCreateRenderPass", (void*)CreateRenderPass},
    {"vkDestroyRenderPass", (void*)DestroyRenderPass},
    {"vkGetRenderAreaGranularity", (void*)GetRenderAreaGranularity},
    {"vkCreateCommandPool", (void*)CreateCommandPool},
    {"vkDestroyCommandPool", (void*)DestroyCommandPool},
    {"vkResetCommandPool", (void*)ResetCommandPool},
    {"vkAllocateCommandBuffers", (void*)AllocateCommandBuffers},
    {"vkFreeCommandBuffers", (void*)FreeCommandBuffers},
    {"vkBeginCommandBuffer", (void*)BeginCommandBuffer},
    {"vkEndCommandBuffer", (void*)EndCommandBuffer},
    {"vkResetCommandBuffer", (void*)ResetCommandBuffer},
    {"vkCmdBindPipeline", (void*)CmdBindPipeline},
    {"vkCmdSetViewport", (void*)CmdSetViewport},
    {"vkCmdSetScissor", (void*)CmdSetScissor},
    {"vkCmdSetLineWidth", (void*)CmdSetLineWidth},
    {"vkCmdSetDepthBias", (void*)CmdSetDepthBias},
    {"vkCmdSetBlendConstants", (void*)CmdSetBlendConstants},
    {"vkCmdSetDepthBounds", (void*)CmdSetDepthBounds},
    {"vkCmdSetStencilCompareMask", (void*)CmdSetStencilCompareMask},
    {"vkCmdSetStencilWriteMask", (void*)CmdSetStencilWriteMask},
    {"vkCmdSetStencilReference", (void*)CmdSetStencilReference},
    {"vkCmdBindDescriptorSets", (void*)CmdBindDescriptorSets},
    {"vkCmdBindIndexBuffer", (void*)CmdBindIndexBuffer},
    {"vkCmdBindVertexBuffers", (void*)CmdBindVertexBuffers},
    {"vkCmdDraw", (void*)CmdDraw},
    {"vkCmdDrawIndexed", (void*)CmdDrawIndexed},
    {"vkCmdDrawIndirect", (void*)CmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (void*)CmdDrawIndexedIndirect},
    {"vkCmdDispatch", (void*)CmdDispatch},
    {"vkCmdDispatchIndirect", (void*)CmdDispatchIndirect},
    {"vkCmdCopyBuffer", (void*)CmdCopyBuffer},
    {"vkCmdCopyImage", (void*)CmdCopyImage},
    {"vkCmdBlitImage", (void*)CmdBlitImage},
    {"vkCmdCopyBufferToImage", (void*)CmdCopyBufferToImage},
    {"vkCmdCopyImageToBuffer", (void*)CmdCopyImageToBuffer},
    {"vkCmdUpdateBuffer", (void*)CmdUpdateBuffer},
    {"vkCmdFillBuffer", (void*)CmdFillBuffer},
    {"vkCmdClearColorImage", (void*)CmdClearColorImage},
    {"vkCmdClearDepthStencilImage", (void*)CmdClearDepthStencilImage},
    {"vkCmdClearAttachments", (void*)CmdClearAttachments},
    {"vkCmdResolveImage", (void*)CmdResolveImage},
    {"vkCmdSetEvent", (void*)CmdSetEvent},
    {"vkCmdResetEvent", (void*)CmdResetEvent},
    {"vkCmdWaitEvents", (void*)CmdWaitEvents},
    {"vkCmdPipelineBarrier", (void*)CmdPipelineBarrier},
    {"vkCmdBeginQuery", (void*)CmdBeginQuery},
    {"vkCmdEndQuery", (void*)CmdEndQuery},
    {"vkCmdResetQueryPool", (void*)CmdResetQueryPool},
    {"vkCmdWriteTimestamp", (void*)CmdWriteTimestamp},
    {"vkCmdCopyQueryPoolResults", (void*)CmdCopyQueryPoolResults},
    {"vkCmdPushConstants", (void*)CmdPushConstants},
    {"vkCmdBeginRenderPass", (void*)CmdBeginRenderPass},
    {"vkCmdNextSubpass", (void*)CmdNextSubpass},
    {"vkCmdEndRenderPass", (void*)CmdEndRenderPass},
    {"vkCmdExecuteCommands", (void*)CmdExecuteCommands},
    {"vkBindBufferMemory2", (void*)BindBufferMemory2},
    {"vkBindImageMemory2", (void*)BindImageMemory2},
    {"vkGetDeviceGroupPeerMemoryFeatures", (void*)GetDeviceGroupPeerMemoryFeatures},
    {"vkCmdSetDeviceMask", (void*)CmdSetDeviceMask},
    {"vkCmdDispatchBase", (void*)CmdDispatchBase},
    {"vkEnumeratePhysicalDeviceGroups", (void*)EnumeratePhysicalDeviceGroups},
    {"vkGetImageMemoryRequirements2", (void*)GetImageMemoryRequirements2},
    {"vkGetBufferMemoryRequirements2", (void*)GetBufferMemoryRequirements2},
    {"vkGetImageSparseMemoryRequirements2", (void*)GetImageSparseMemoryRequirements2},
    {"vkGetPhysicalDeviceFeatures2", (void*)GetPhysicalDeviceFeatures2},
    {"vkGetPhysicalDeviceProperties2", (void*)GetPhysicalDeviceProperties2},
    {"vkGetPhysicalDeviceFormatProperties2", (void*)GetPhysicalDeviceFormatProperties2},
    {"vkGetPhysicalDeviceImageFormatProperties2", (void*)GetPhysicalDeviceImageFormatProperties2},
    {"vkGetPhysicalDeviceQueueFamilyProperties2", (void*)GetPhysicalDeviceQueueFamilyProperties2},
    {"vkGetPhysicalDeviceMemoryProperties2", (void*)GetPhysicalDeviceMemoryProperties2},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2", (void*)GetPhysicalDeviceSparseImageFormatProperties2},
    {"vkTrimCommandPool", (void*)TrimCommandPool},
    {"vkGetDeviceQueue2", (void*)GetDeviceQueue2},
    {"vkCreateSamplerYcbcrConversion", (void*)CreateSamplerYcbcrConversion},
    {"vkDestroySamplerYcbcrConversion", (void*)DestroySamplerYcbcrConversion},
    {"vkCreateDescriptorUpdateTemplate", (void*)CreateDescriptorUpdateTemplate},
    {"vkDestroyDescriptorUpdateTemplate", (void*)DestroyDescriptorUpdateTemplate},
    {"vkUpdateDescriptorSetWithTemplate", (void*)UpdateDescriptorSetWithTemplate},
    {"vkGetPhysicalDeviceExternalBufferProperties", (void*)GetPhysicalDeviceExternalBufferProperties},
    {"vkGetPhysicalDeviceExternalFenceProperties", (void*)GetPhysicalDeviceExternalFenceProperties},
    {"vkGetPhysicalDeviceExternalSemaphoreProperties", (void*)GetPhysicalDeviceExternalSemaphoreProperties},
    {"vkGetDescriptorSetLayoutSupport", (void*)GetDescriptorSetLayoutSupport},
    {"vkDestroySurfaceKHR", (void*)DestroySurfaceKHR},
    {"vkGetPhysicalDeviceSurfaceSupportKHR", (void*)GetPhysicalDeviceSurfaceSupportKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilitiesKHR", (void*)GetPhysicalDeviceSurfaceCapabilitiesKHR},
    {"vkGetPhysicalDeviceSurfaceFormatsKHR", (void*)GetPhysicalDeviceSurfaceFormatsKHR},
    {"vkGetPhysicalDeviceSurfacePresentModesKHR", (void*)GetPhysicalDeviceSurfacePresentModesKHR},
    {"vkCreateSwapchainKHR", (void*)CreateSwapchainKHR},
    {"vkDestroySwapchainKHR", (void*)DestroySwapchainKHR},
    {"vkGetSwapchainImagesKHR", (void*)GetSwapchainImagesKHR},
    {"vkAcquireNextImageKHR", (void*)AcquireNextImageKHR},
    {"vkQueuePresentKHR", (void*)QueuePresentKHR},
    {"vkGetDeviceGroupPresentCapabilitiesKHR", (void*)GetDeviceGroupPresentCapabilitiesKHR},
    {"vkGetDeviceGroupSurfacePresentModesKHR", (void*)GetDeviceGroupSurfacePresentModesKHR},
    {"vkGetPhysicalDevicePresentRectanglesKHR", (void*)GetPhysicalDevicePresentRectanglesKHR},
    {"vkAcquireNextImage2KHR", (void*)AcquireNextImage2KHR},
    {"vkGetPhysicalDeviceDisplayPropertiesKHR", (void*)GetPhysicalDeviceDisplayPropertiesKHR},
    {"vkGetPhysicalDeviceDisplayPlanePropertiesKHR", (void*)GetPhysicalDeviceDisplayPlanePropertiesKHR},
    {"vkGetDisplayPlaneSupportedDisplaysKHR", (void*)GetDisplayPlaneSupportedDisplaysKHR},
    {"vkGetDisplayModePropertiesKHR", (void*)GetDisplayModePropertiesKHR},
    {"vkCreateDisplayModeKHR", (void*)CreateDisplayModeKHR},
    {"vkGetDisplayPlaneCapabilitiesKHR", (void*)GetDisplayPlaneCapabilitiesKHR},
    {"vkCreateDisplayPlaneSurfaceKHR", (void*)CreateDisplayPlaneSurfaceKHR},
    {"vkCreateSharedSwapchainsKHR", (void*)CreateSharedSwapchainsKHR},
#ifdef VK_USE_PLATFORM_XLIB_KHR
    {"vkCreateXlibSurfaceKHR", (void*)CreateXlibSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    {"vkGetPhysicalDeviceXlibPresentationSupportKHR", (void*)GetPhysicalDeviceXlibPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    {"vkCreateXcbSurfaceKHR", (void*)CreateXcbSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    {"vkGetPhysicalDeviceXcbPresentationSupportKHR", (void*)GetPhysicalDeviceXcbPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    {"vkCreateWaylandSurfaceKHR", (void*)CreateWaylandSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    {"vkGetPhysicalDeviceWaylandPresentationSupportKHR", (void*)GetPhysicalDeviceWaylandPresentationSupportKHR},
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkCreateAndroidSurfaceKHR", (void*)CreateAndroidSurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkCreateWin32SurfaceKHR", (void*)CreateWin32SurfaceKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetPhysicalDeviceWin32PresentationSupportKHR", (void*)GetPhysicalDeviceWin32PresentationSupportKHR},
#endif
    {"vkGetPhysicalDeviceFeatures2KHR", (void*)GetPhysicalDeviceFeatures2KHR},
    {"vkGetPhysicalDeviceProperties2KHR", (void*)GetPhysicalDeviceProperties2KHR},
    {"vkGetPhysicalDeviceFormatProperties2KHR", (void*)GetPhysicalDeviceFormatProperties2KHR},
    {"vkGetPhysicalDeviceImageFormatProperties2KHR", (void*)GetPhysicalDeviceImageFormatProperties2KHR},
    {"vkGetPhysicalDeviceQueueFamilyProperties2KHR", (void*)GetPhysicalDeviceQueueFamilyProperties2KHR},
    {"vkGetPhysicalDeviceMemoryProperties2KHR", (void*)GetPhysicalDeviceMemoryProperties2KHR},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2KHR", (void*)GetPhysicalDeviceSparseImageFormatProperties2KHR},
    {"vkGetDeviceGroupPeerMemoryFeaturesKHR", (void*)GetDeviceGroupPeerMemoryFeaturesKHR},
    {"vkCmdSetDeviceMaskKHR", (void*)CmdSetDeviceMaskKHR},
    {"vkCmdDispatchBaseKHR", (void*)CmdDispatchBaseKHR},
    {"vkTrimCommandPoolKHR", (void*)TrimCommandPoolKHR},
    {"vkEnumeratePhysicalDeviceGroupsKHR", (void*)EnumeratePhysicalDeviceGroupsKHR},
    {"vkGetPhysicalDeviceExternalBufferPropertiesKHR", (void*)GetPhysicalDeviceExternalBufferPropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandleKHR", (void*)GetMemoryWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandlePropertiesKHR", (void*)GetMemoryWin32HandlePropertiesKHR},
#endif
    {"vkGetMemoryFdKHR", (void*)GetMemoryFdKHR},
    {"vkGetMemoryFdPropertiesKHR", (void*)GetMemoryFdPropertiesKHR},
    {"vkGetPhysicalDeviceExternalSemaphorePropertiesKHR", (void*)GetPhysicalDeviceExternalSemaphorePropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkImportSemaphoreWin32HandleKHR", (void*)ImportSemaphoreWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetSemaphoreWin32HandleKHR", (void*)GetSemaphoreWin32HandleKHR},
#endif
    {"vkImportSemaphoreFdKHR", (void*)ImportSemaphoreFdKHR},
    {"vkGetSemaphoreFdKHR", (void*)GetSemaphoreFdKHR},
    {"vkCmdPushDescriptorSetKHR", (void*)CmdPushDescriptorSetKHR},
    {"vkCmdPushDescriptorSetWithTemplateKHR", (void*)CmdPushDescriptorSetWithTemplateKHR},
    {"vkCreateDescriptorUpdateTemplateKHR", (void*)CreateDescriptorUpdateTemplateKHR},
    {"vkDestroyDescriptorUpdateTemplateKHR", (void*)DestroyDescriptorUpdateTemplateKHR},
    {"vkUpdateDescriptorSetWithTemplateKHR", (void*)UpdateDescriptorSetWithTemplateKHR},
    {"vkCreateRenderPass2KHR", (void*)CreateRenderPass2KHR},
    {"vkCmdBeginRenderPass2KHR", (void*)CmdBeginRenderPass2KHR},
    {"vkCmdNextSubpass2KHR", (void*)CmdNextSubpass2KHR},
    {"vkCmdEndRenderPass2KHR", (void*)CmdEndRenderPass2KHR},
    {"vkGetSwapchainStatusKHR", (void*)GetSwapchainStatusKHR},
    {"vkGetPhysicalDeviceExternalFencePropertiesKHR", (void*)GetPhysicalDeviceExternalFencePropertiesKHR},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkImportFenceWin32HandleKHR", (void*)ImportFenceWin32HandleKHR},
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetFenceWin32HandleKHR", (void*)GetFenceWin32HandleKHR},
#endif
    {"vkImportFenceFdKHR", (void*)ImportFenceFdKHR},
    {"vkGetFenceFdKHR", (void*)GetFenceFdKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilities2KHR", (void*)GetPhysicalDeviceSurfaceCapabilities2KHR},
    {"vkGetPhysicalDeviceSurfaceFormats2KHR", (void*)GetPhysicalDeviceSurfaceFormats2KHR},
    {"vkGetPhysicalDeviceDisplayProperties2KHR", (void*)GetPhysicalDeviceDisplayProperties2KHR},
    {"vkGetPhysicalDeviceDisplayPlaneProperties2KHR", (void*)GetPhysicalDeviceDisplayPlaneProperties2KHR},
    {"vkGetDisplayModeProperties2KHR", (void*)GetDisplayModeProperties2KHR},
    {"vkGetDisplayPlaneCapabilities2KHR", (void*)GetDisplayPlaneCapabilities2KHR},
    {"vkGetImageMemoryRequirements2KHR", (void*)GetImageMemoryRequirements2KHR},
    {"vkGetBufferMemoryRequirements2KHR", (void*)GetBufferMemoryRequirements2KHR},
    {"vkGetImageSparseMemoryRequirements2KHR", (void*)GetImageSparseMemoryRequirements2KHR},
    {"vkCreateSamplerYcbcrConversionKHR", (void*)CreateSamplerYcbcrConversionKHR},
    {"vkDestroySamplerYcbcrConversionKHR", (void*)DestroySamplerYcbcrConversionKHR},
    {"vkBindBufferMemory2KHR", (void*)BindBufferMemory2KHR},
    {"vkBindImageMemory2KHR", (void*)BindImageMemory2KHR},
    {"vkGetDescriptorSetLayoutSupportKHR", (void*)GetDescriptorSetLayoutSupportKHR},
    {"vkCmdDrawIndirectCountKHR", (void*)CmdDrawIndirectCountKHR},
    {"vkCmdDrawIndexedIndirectCountKHR", (void*)CmdDrawIndexedIndirectCountKHR},
    {"vkCreateDebugReportCallbackEXT", (void*)CreateDebugReportCallbackEXT},
    {"vkDestroyDebugReportCallbackEXT", (void*)DestroyDebugReportCallbackEXT},
    {"vkDebugReportMessageEXT", (void*)DebugReportMessageEXT},
    {"vkDebugMarkerSetObjectTagEXT", (void*)DebugMarkerSetObjectTagEXT},
    {"vkDebugMarkerSetObjectNameEXT", (void*)DebugMarkerSetObjectNameEXT},
    {"vkCmdDebugMarkerBeginEXT", (void*)CmdDebugMarkerBeginEXT},
    {"vkCmdDebugMarkerEndEXT", (void*)CmdDebugMarkerEndEXT},
    {"vkCmdDebugMarkerInsertEXT", (void*)CmdDebugMarkerInsertEXT},
    {"vkCmdBindTransformFeedbackBuffersEXT", (void*)CmdBindTransformFeedbackBuffersEXT},
    {"vkCmdBeginTransformFeedbackEXT", (void*)CmdBeginTransformFeedbackEXT},
    {"vkCmdEndTransformFeedbackEXT", (void*)CmdEndTransformFeedbackEXT},
    {"vkCmdBeginQueryIndexedEXT", (void*)CmdBeginQueryIndexedEXT},
    {"vkCmdEndQueryIndexedEXT", (void*)CmdEndQueryIndexedEXT},
    {"vkCmdDrawIndirectByteCountEXT", (void*)CmdDrawIndirectByteCountEXT},
    {"vkCmdDrawIndirectCountAMD", (void*)CmdDrawIndirectCountAMD},
    {"vkCmdDrawIndexedIndirectCountAMD", (void*)CmdDrawIndexedIndirectCountAMD},
    {"vkGetShaderInfoAMD", (void*)GetShaderInfoAMD},
    {"vkGetPhysicalDeviceExternalImageFormatPropertiesNV", (void*)GetPhysicalDeviceExternalImageFormatPropertiesNV},
#ifdef VK_USE_PLATFORM_WIN32_KHR
    {"vkGetMemoryWin32HandleNV", (void*)GetMemoryWin32HandleNV},
#endif
#ifdef VK_USE_PLATFORM_VI_NN
    {"vkCreateViSurfaceNN", (void*)CreateViSurfaceNN},
#endif
    {"vkCmdBeginConditionalRenderingEXT", (void*)CmdBeginConditionalRenderingEXT},
    {"vkCmdEndConditionalRenderingEXT", (void*)CmdEndConditionalRenderingEXT},
    {"vkCmdProcessCommandsNVX", (void*)CmdProcessCommandsNVX},
    {"vkCmdReserveSpaceForCommandsNVX", (void*)CmdReserveSpaceForCommandsNVX},
    {"vkCreateIndirectCommandsLayoutNVX", (void*)CreateIndirectCommandsLayoutNVX},
    {"vkDestroyIndirectCommandsLayoutNVX", (void*)DestroyIndirectCommandsLayoutNVX},
    {"vkCreateObjectTableNVX", (void*)CreateObjectTableNVX},
    {"vkDestroyObjectTableNVX", (void*)DestroyObjectTableNVX},
    {"vkRegisterObjectsNVX", (void*)RegisterObjectsNVX},
    {"vkUnregisterObjectsNVX", (void*)UnregisterObjectsNVX},
    {"vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX", (void*)GetPhysicalDeviceGeneratedCommandsPropertiesNVX},
    {"vkCmdSetViewportWScalingNV", (void*)CmdSetViewportWScalingNV},
    {"vkReleaseDisplayEXT", (void*)ReleaseDisplayEXT},
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    {"vkAcquireXlibDisplayEXT", (void*)AcquireXlibDisplayEXT},
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    {"vkGetRandROutputDisplayEXT", (void*)GetRandROutputDisplayEXT},
#endif
    {"vkGetPhysicalDeviceSurfaceCapabilities2EXT", (void*)GetPhysicalDeviceSurfaceCapabilities2EXT},
    {"vkDisplayPowerControlEXT", (void*)DisplayPowerControlEXT},
    {"vkRegisterDeviceEventEXT", (void*)RegisterDeviceEventEXT},
    {"vkRegisterDisplayEventEXT", (void*)RegisterDisplayEventEXT},
    {"vkGetSwapchainCounterEXT", (void*)GetSwapchainCounterEXT},
    {"vkGetRefreshCycleDurationGOOGLE", (void*)GetRefreshCycleDurationGOOGLE},
    {"vkGetPastPresentationTimingGOOGLE", (void*)GetPastPresentationTimingGOOGLE},
    {"vkCmdSetDiscardRectangleEXT", (void*)CmdSetDiscardRectangleEXT},
    {"vkSetHdrMetadataEXT", (void*)SetHdrMetadataEXT},
#ifdef VK_USE_PLATFORM_IOS_MVK
    {"vkCreateIOSSurfaceMVK", (void*)CreateIOSSurfaceMVK},
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    {"vkCreateMacOSSurfaceMVK", (void*)CreateMacOSSurfaceMVK},
#endif
    {"vkSetDebugUtilsObjectNameEXT", (void*)SetDebugUtilsObjectNameEXT},
    {"vkSetDebugUtilsObjectTagEXT", (void*)SetDebugUtilsObjectTagEXT},
    {"vkQueueBeginDebugUtilsLabelEXT", (void*)QueueBeginDebugUtilsLabelEXT},
    {"vkQueueEndDebugUtilsLabelEXT", (void*)QueueEndDebugUtilsLabelEXT},
    {"vkQueueInsertDebugUtilsLabelEXT", (void*)QueueInsertDebugUtilsLabelEXT},
    {"vkCmdBeginDebugUtilsLabelEXT", (void*)CmdBeginDebugUtilsLabelEXT},
    {"vkCmdEndDebugUtilsLabelEXT", (void*)CmdEndDebugUtilsLabelEXT},
    {"vkCmdInsertDebugUtilsLabelEXT", (void*)CmdInsertDebugUtilsLabelEXT},
    {"vkCreateDebugUtilsMessengerEXT", (void*)CreateDebugUtilsMessengerEXT},
    {"vkDestroyDebugUtilsMessengerEXT", (void*)DestroyDebugUtilsMessengerEXT},
    {"vkSubmitDebugUtilsMessageEXT", (void*)SubmitDebugUtilsMessageEXT},
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkGetAndroidHardwareBufferPropertiesANDROID", (void*)GetAndroidHardwareBufferPropertiesANDROID},
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    {"vkGetMemoryAndroidHardwareBufferANDROID", (void*)GetMemoryAndroidHardwareBufferANDROID},
#endif
    {"vkCmdSetSampleLocationsEXT", (void*)CmdSetSampleLocationsEXT},
    {"vkGetPhysicalDeviceMultisamplePropertiesEXT", (void*)GetPhysicalDeviceMultisamplePropertiesEXT},
    {"vkGetImageDrmFormatModifierPropertiesEXT", (void*)GetImageDrmFormatModifierPropertiesEXT},
    {"vkCreateValidationCacheEXT", (void*)CreateValidationCacheEXT},
    {"vkDestroyValidationCacheEXT", (void*)DestroyValidationCacheEXT},
    {"vkMergeValidationCachesEXT", (void*)MergeValidationCachesEXT},
    {"vkGetValidationCacheDataEXT", (void*)GetValidationCacheDataEXT},
    {"vkCmdBindShadingRateImageNV", (void*)CmdBindShadingRateImageNV},
    {"vkCmdSetViewportShadingRatePaletteNV", (void*)CmdSetViewportShadingRatePaletteNV},
    {"vkCmdSetCoarseSampleOrderNV", (void*)CmdSetCoarseSampleOrderNV},
    {"vkCreateAccelerationStructureNV", (void*)CreateAccelerationStructureNV},
    {"vkDestroyAccelerationStructureNV", (void*)DestroyAccelerationStructureNV},
    {"vkGetAccelerationStructureMemoryRequirementsNV", (void*)GetAccelerationStructureMemoryRequirementsNV},
    {"vkBindAccelerationStructureMemoryNV", (void*)BindAccelerationStructureMemoryNV},
    {"vkCmdBuildAccelerationStructureNV", (void*)CmdBuildAccelerationStructureNV},
    {"vkCmdCopyAccelerationStructureNV", (void*)CmdCopyAccelerationStructureNV},
    {"vkCmdTraceRaysNV", (void*)CmdTraceRaysNV},
    {"vkCreateRayTracingPipelinesNV", (void*)CreateRayTracingPipelinesNV},
    {"vkGetRayTracingShaderGroupHandlesNV", (void*)GetRayTracingShaderGroupHandlesNV},
    {"vkGetAccelerationStructureHandleNV", (void*)GetAccelerationStructureHandleNV},
    {"vkCmdWriteAccelerationStructuresPropertiesNV", (void*)CmdWriteAccelerationStructuresPropertiesNV},
    {"vkCompileDeferredNV", (void*)CompileDeferredNV},
    {"vkGetMemoryHostPointerPropertiesEXT", (void*)GetMemoryHostPointerPropertiesEXT},
    {"vkCmdWriteBufferMarkerAMD", (void*)CmdWriteBufferMarkerAMD},
    {"vkGetPhysicalDeviceCalibrateableTimeDomainsEXT", (void*)GetPhysicalDeviceCalibrateableTimeDomainsEXT},
    {"vkGetCalibratedTimestampsEXT", (void*)GetCalibratedTimestampsEXT},
    {"vkCmdDrawMeshTasksNV", (void*)CmdDrawMeshTasksNV},
    {"vkCmdDrawMeshTasksIndirectNV", (void*)CmdDrawMeshTasksIndirectNV},
    {"vkCmdDrawMeshTasksIndirectCountNV", (void*)CmdDrawMeshTasksIndirectCountNV},
    {"vkCmdSetExclusiveScissorNV", (void*)CmdSetExclusiveScissorNV},
    {"vkCmdSetCheckpointNV", (void*)CmdSetCheckpointNV},
    {"vkGetQueueCheckpointDataNV", (void*)GetQueueCheckpointDataNV},
#ifdef VK_USE_PLATFORM_FUCHSIA
    {"vkCreateImagePipeSurfaceFUCHSIA", (void*)CreateImagePipeSurfaceFUCHSIA},
#endif
    {"vkGetBufferDeviceAddressEXT", (void*)GetBufferDeviceAddressEXT},
};


} // namespace vulkan_layer_chassis

// loader-layer interface v0, just wrappers since there is only a layer

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                                      VkExtensionProperties *pProperties) {
    return vulkan_layer_chassis::EnumerateInstanceExtensionProperties(pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pCount,
                                                                                  VkLayerProperties *pProperties) {
    return vulkan_layer_chassis::EnumerateInstanceLayerProperties(pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount,
                                                                                VkLayerProperties *pProperties) {
    // the layer command handles VK_NULL_HANDLE just fine internally
    assert(physicalDevice == VK_NULL_HANDLE);
    return vulkan_layer_chassis::EnumerateDeviceLayerProperties(VK_NULL_HANDLE, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                    const char *pLayerName, uint32_t *pCount,
                                                                                    VkExtensionProperties *pProperties) {
    // the layer command handles VK_NULL_HANDLE just fine internally
    assert(physicalDevice == VK_NULL_HANDLE);
    return vulkan_layer_chassis::EnumerateDeviceExtensionProperties(VK_NULL_HANDLE, pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev, const char *funcName) {
    return vulkan_layer_chassis::GetDeviceProcAddr(dev, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *funcName) {
    return vulkan_layer_chassis::GetInstanceProcAddr(instance, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_layerGetPhysicalDeviceProcAddr(VkInstance instance,
                                                                                           const char *funcName) {
    return vulkan_layer_chassis::GetPhysicalDeviceProcAddr(instance, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface *pVersionStruct) {
    assert(pVersionStruct != NULL);
    assert(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

    // Fill in the function pointers if our version is at least capable of having the structure contain them.
    if (pVersionStruct->loaderLayerInterfaceVersion >= 2) {
        pVersionStruct->pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = vkGetDeviceProcAddr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = vk_layerGetPhysicalDeviceProcAddr;
    }

    return VK_SUCCESS;
}
