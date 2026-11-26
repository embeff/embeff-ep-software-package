# Copyright (C) 2026 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

# Captured here (rather than inside the function below) because
# CMAKE_CURRENT_LIST_DIR reflects the caller's directory when evaluated
# inside a function.
set(EP_OPENMODELICA_HELPER_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Builds an FMU from a Modelica model via OpenModelica and registers it as a
# build target named `target_name`. Can be called once per Modelica model
# that needs exporting.
#
function(ep_add_openmodelica_fmu target_name model_name model_file output_dir)
    find_program(OMC_EXECUTABLE omc)
    if(NOT OMC_EXECUTABLE)
        message(WARNING
            "OpenModelica compiler (omc) not found — skipping FMU target "
            "'${target_name}' for model '${model_name}'.")
        return()
    endif()

    set(FMU_MODEL_NAME "${model_name}")
    set(mos_script "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.mos")
    configure_file("${EP_OPENMODELICA_HELPER_DIR}/buildFMU.mos.in" "${mos_script}")

    set(fmu_output "${output_dir}/${model_name}.fmu")

    add_custom_command(
        OUTPUT "${fmu_output}"
        COMMAND ${OMC_EXECUTABLE} ${mos_script} ${model_file}
        DEPENDS ${model_file} ${mos_script}
        WORKING_DIRECTORY "${output_dir}"
        COMMENT "Generating FMU for ${model_name}"
    )
    add_custom_target(${target_name} ALL DEPENDS "${fmu_output}")
endfunction(ep_add_openmodelica_fmu)
